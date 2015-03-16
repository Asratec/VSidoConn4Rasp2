/*
Copyright (c) 2015, Asratec Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of VSidoConn4Edison nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <string>
#include <iostream>
#include <list>
using namespace std;
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h> 

#include "debug.h"


#include "wscomm.hpp"

#include <mutex>              // std::mutex, std::lock_guard
using namespace std;
static mutex mtxMarker;

static mutex mtxConnection;



#include <libwebsockets.h>


void recieveMessage(const string &json);
void responseWS(const string &msg);

static int callback_http(struct libwebsocket_context * ctx,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    return 0;
}


static list<struct libwebsocket *> callbacks;

static int callback_ws_info(struct libwebsocket_context * ctx,
                                   struct libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user, void *inMsg, size_t lenMsg)
{
   
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
        	DUMP_VAR(wsi);
        	if(nullptr == wsi)
        	{
        		return 0;
        	}
    		{
    			lock_guard<mutex> lock(mtxConnection);
    			callbacks.push_back(wsi);
    		}
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
        	if(nullptr == wsi)
        	{
        		DUMP_VAR(wsi);
        		return 0;
        	}
        	string msgStr((const char*)inMsg,lenMsg);
        	try
        	{
				DUMP_VAR(msgStr);
        		recieveMessage(msgStr);
        	}
        	catch(...)
        	{
        		printf("LWS_CALLBACK_RECEIVE exception %s:%d\n",__FILE__,__LINE__);
        	}
            break;
        }
    	case LWS_CALLBACK_CLOSED:
    	{
            printf("connection closed\n");
        	DUMP_VAR(wsi);
        	if(nullptr == wsi)
        	{
        		return 0;
        	}
    		{
    			lock_guard<mutex> lock(mtxConnection);
    			callbacks.remove(wsi);
    		}
    		break;
    	}
        default:
            break;
    }
   
   
    return 0;
}

static struct libwebsocket_protocols protocols[] = 
{
    /* first protocol must always be HTTP handler */
    {
        "http-only",   // name
        callback_http, // callback
        0              // per_session_data_size
    },
    {
        "webcam-info", // protocol name - very important!
        callback_ws_info,   // callback
        0                          // we don't use any per session data

    },
    {
        "r2r-comm", // protocol name - very important!
        callback_ws_info,   // callback
        0                          // we don't use any per session data
    },
    {
        NULL, NULL, 0   /* End of list */
    }
};


struct libwebsocket_context *gContext;

/** WebSocket要求受付スレッド本体
* @return None
*/
void doWebSocket(void)
{

	struct libwebsocket_context *context;
	
	lws_context_creation_info info ={0}; 
	info.port = 20088;
	info.protocols = protocols;

	info.gid = -1;
	info.uid = -1;
	
	// create libwebsocket context representing this server
	context = libwebsocket_create_context(&info);

	if (context == NULL)
	{
		fprintf(stderr, "libwebsocket init failed\n");
		return;
	}

	printf("starting server...\n");
	
	gContext = context;

	// infinite loop, to end this server send SIGTERM. (CTRL+C)
	while (true)
	{
		libwebsocket_service(context, 100);
	}

	libwebsocket_context_destroy(context);
   
}

extern int col_ch1_lower;
extern int col_ch1_upper; //色認識での色相のしきい値(小),（大）
extern int col_ch2_lower;
extern int col_ch2_upper; //色認識での彩度のしきい値(小),（大）
extern int col_ch3_lower;
extern int col_ch3_upper; //色認識での輝度のしきい値(小),（大）

#include "picojson.h"


#include <sys/time.h>




static string lastMarker = "";
static long long  markerTimestamp = 2000;
static void tryKeepMarker(const string &json )
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	markerTimestamp = tv.tv_sec *1000 + tv.tv_usec /1000;
	lastMarker = json;
}


void sendR2R(picojson::value &raw);
void writeR2RSocket(const string &msg,const string &ip);


void recieveMessage(const string &json)
{
    picojson::value _rawV;
	try
	{
		std::string err;
		picojson::parse(_rawV, json.c_str(),json.c_str() + json.size(),&err);
		DUMP_VAR(err);
		DUMP_VAR(_rawV);
		auto ser = _rawV.serialize();
		DUMP_VAR(ser);
		
		picojson::object _raw = _rawV.get<picojson::object>();
		
		if(_rawV.contains("markerColor"))
		{
			auto color = _raw["markerColor"].get<picojson::object>();
			auto ch1L = color["HueL"].get<double>();
			col_ch1_lower = (int )ch1L;
			DUMP_VAR(ch1L);
			auto ch1U = color["HueU"].get<double>();
			DUMP_VAR(ch1U);
			col_ch1_upper = (int )ch1U;
			

			auto ch2L = color["SatL"].get<double>();
			DUMP_VAR(ch2L);
			col_ch2_lower = (int )ch2L;
			auto ch2U = color["SatU"].get<double>();
			DUMP_VAR(ch2U);
			col_ch2_upper = (int )ch2U;
			
			auto ch3L = color["ValL"].get<double>();
			DUMP_VAR(ch3L);
			col_ch3_lower = (int )ch3L;
			auto ch3U = color["ValU"].get<double>();
			DUMP_VAR(ch3U);
			col_ch3_upper = (int )ch3U;
			
			
		}
		if(_rawV.contains("r2r"))
		{
			sendR2R(_rawV);
		}
		
	}
	catch(const std::runtime_error &e)
	{
		DUMP_VAR(e.what());
	}
}

void responseWS(const string &msg)
{
	lock_guard<mutex> lock(mtxConnection);
	DUMP_VAR(callbacks.size());
	list<struct libwebsocket *> badCB;
	for(auto wsi : callbacks)
	{
		struct timeval timeout;      
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000 * 500;
		int sockfd = libwebsocket_get_socket_fd(wsi);
		if(0 > sockfd)
		{
    		perror("libwebsocket_get_socket_fd\n");
		}
		else
		{
			printf("%s,%d,sockfd=<%d>\n",__FILE__,__LINE__,sockfd);
		}
		if(setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
		{
    		perror("setsockopt failed\n");
		}
		auto ret = libwebsocket_write(wsi, (unsigned char*)msg.c_str(), msg.size(), LWS_WRITE_TEXT);
		DUMP_VAR(ret);
		if(ret <msg.size() )
		{
			badCB.push_back(wsi);
		}
	}
	for(auto wsi : badCB)
	{
		callbacks.remove(wsi);
	}
}

void notifyMarker(const string &json)
{
	{
		DUMP_VAR(json);
		lock_guard<mutex> lock(mtxMarker);
		tryKeepMarker(json);
	}
	responseWS(json);
}


void notifyR2R(const string &json)
{
	responseWS(json);
}

void sendR2R(picojson::value &_rawV)
{
	try
	{
		picojson::object _raw = _rawV.get<picojson::object>();
		string ip;
		if(_rawV.contains("dist"))
		{
			ip = _raw["dist"].get<string>();
		}
		DUMP_VAR(ip);
		if(ip.empty())
		{
			return ;
		}
		if(_raw["r2r"].contains("attack"))
		{
			picojson::object r2r = _raw["r2r"].get<picojson::object>();
			picojson::object attack = r2r["attack"].get<picojson::object>();
			struct timeval tv;
			gettimeofday(&tv,NULL);
			long long now = tv.tv_sec *1000 + tv.tv_usec /1000;
			if(1000 > now -markerTimestamp)
			{
				lock_guard<mutex> lock(mtxMarker);

				std::string errMarker;
				picojson::value _Vmarker;
				DUMP_VAR(lastMarker);
				picojson::parse(_Vmarker, lastMarker.c_str(),lastMarker.c_str() + lastMarker.size(),&errMarker);
				DUMP_VAR(errMarker);
				if(errMarker.empty())
				{
					picojson::object _marker = _Vmarker.get<picojson::object>();
					attack["marker"] = _marker["marker"];
				}
			}
			r2r["attack"] = picojson::value(attack);
			_raw["r2r"] = picojson::value(r2r);
		}
		picojson::value r2rV(_raw);
		auto r2r = r2rV.serialize();
		DUMP_VAR(r2r);
		writeR2RSocket(r2r,ip);
	}
	catch(const std::runtime_error &e)
	{
		DUMP_VAR(e.what());
	}
}


#include <string>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>





#if 1
	#define _PERROR perror
#else
	#define _PERROR(x) 
#endif

void writeR2RSocket(const string &msg,const string &ip)
{
	int sockfd;
    struct sockaddr_in servaddr;
 
    sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 >= sockfd)
	{
		_PERROR("socket");
		return;
	}
    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22100);
	
	int ret = inet_pton(AF_INET,ip.c_str(),&(servaddr.sin_addr));
	if(0 > ret)
	{
		_PERROR("inet_pton");
		return;
	}
    ret = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(0 > ret)
	{
		_PERROR("connect");
		return;
	}
	
	
    /// read ack
	char data[1024] = {0};

	struct timeval timeout;      
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000 * 500;
	if(setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
	{
		_PERROR("setsockopt failed\n");
		return;
	}
	
    ::write(sockfd,msg.c_str(),msg.size());
    
    close(sockfd);
}



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
#include <vector>
#include <map>
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
#include <string.h> 
#include <unistd.h> 

#include "debug.h"


#include "wscomm.hpp"

#include <mutex>              // std::mutex, std::lock_guard
using namespace std;
static mutex mtxMarker;

static mutex mtxConnection;


#include <libwebsockets.h>


static mutex mtxRecive;


static mutex mtxWSAck;
static map<libwebsocket *,list<string>> globalAckList;


void recieveMessage(const string json);
void responseWS(const string msg);

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
  	DUMP_VAR(reason);
	string reasonMSG;
   
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
    		reasonMSG = "LWS_CALLBACK_ESTABLISHED";
        	DUMP_VAR(reasonMSG);
        	if(nullptr == wsi)
        	{
        		return 0;
        	}
    		{
    			lock_guard<mutex> lock(mtxConnection);
    			callbacks.push_back(wsi);
    		}
    		libwebsocket_callback_on_writable(ctx, wsi);
    		reasonMSG = "LWS_CALLBACK_ESTABLISHED";
        	DUMP_VAR(reasonMSG);
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
    		reasonMSG = "LWS_CALLBACK_RECEIVE";
        	DUMP_VAR(reasonMSG);
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
    		reasonMSG = "LWS_CALLBACK_RECEIVE";
        	DUMP_VAR(reasonMSG);
            break;
        }
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
    		reasonMSG = "LWS_CALLBACK_SERVER_WRITEABLE";
        	DUMP_VAR(reasonMSG);
			lock_guard<mutex> lock(mtxWSAck);
			auto queAck = globalAckList.find(wsi);
			if(globalAckList.end() == queAck)
			{
	    		reasonMSG = "LWS_CALLBACK_SERVER_WRITEABLE";
	        	DUMP_VAR(reasonMSG);
				break;
			}
			if(queAck->second.empty())
			{
	    		reasonMSG = "LWS_CALLBACK_SERVER_WRITEABLE";
	        	DUMP_VAR(reasonMSG);
				break;
			}
			auto msg = queAck->second.front();
	        DUMP_VAR(msg.size());
			if(msg.empty() || 1024 *10 <  msg.size() )
			{
				break;
			}
	        unsigned char *data = new unsigned char [LWS_SEND_BUFFER_PRE_PADDING + msg.size() + LWS_SEND_BUFFER_POST_PADDING];
	        DUMP_VAR((int)data);
			memcpy(&data[LWS_SEND_BUFFER_PRE_PADDING],msg.c_str(),msg.size());
			auto ret = libwebsocket_write(wsi, &data[LWS_SEND_BUFFER_PRE_PADDING], msg.size(), LWS_WRITE_TEXT);
	        if(ret < msg.size())
			{
				DUMP_VAR(wsi);
			}
	        delete []data;
			libwebsocket_callback_on_writable(ctx,wsi);
			queAck->second.pop_front();
    		reasonMSG = "LWS_CALLBACK_SERVER_WRITEABLE";
        	DUMP_VAR(reasonMSG);
			break;
		}
    	case LWS_CALLBACK_CLOSED:
    	{
    		reasonMSG = "LWS_CALLBACK_CLOSED";
        	DUMP_VAR(reasonMSG);
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
    		reasonMSG = "LWS_CALLBACK_CLOSED";
        	DUMP_VAR(reasonMSG);
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
		libwebsocket_service(context, 1000);
	}

	libwebsocket_context_destroy(context);
   
}

extern int col_ch1_lower;
extern int col_ch1_upper; //色認識での色相のしきい値(小),（大）
extern int col_ch2_lower;
extern int col_ch2_upper; //色認識での彩度のしきい値(小),（大）
extern int col_ch3_lower;
extern int col_ch3_upper; //色認識での輝度のしきい値(小),（大）

void updateMarkerColor(void);


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
void writeR2RSocket(const string msg,const string ip);


void recieveMessage(const string json)
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

void responseWS(const string msg)
{
	lock_guard<mutex> lock(mtxConnection);
	DUMP_VAR(callbacks.size());
	list<struct libwebsocket *> badCB;
	for(auto wsi : callbacks)
	{
#if 0
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
#endif
		lock_guard<mutex> lock(mtxWSAck);
		auto que = globalAckList.find(wsi);
		if(globalAckList.end() != que)
		{
			que->second.push_back(msg);
			libwebsocket_callback_on_writable_all_protocol(libwebsockets_get_protocol(wsi));
			DUMP_VAR(wsi);
		}
		else
		{
			list<string> acks = {msg};
			globalAckList[wsi] = acks;
			libwebsocket_callback_on_writable_all_protocol(libwebsockets_get_protocol(wsi));
			DUMP_VAR(wsi);
		}
		/*
		
		 unsigned char *data = new unsigned char [LWS_SEND_BUFFER_PRE_PADDING + msg.size() + LWS_SEND_BUFFER_POST_PADDING];
        memcpy(&data[LWS_SEND_BUFFER_PRE_PADDING],msg.c_str(),msg.size());
		auto ret = libwebsocket_write(wsi, (unsigned char*)&data[LWS_SEND_BUFFER_PRE_PADDING], msg.size(), LWS_WRITE_TEXT);
		DUMP_VAR(ret);
		if(ret <msg.size() )
		{
			badCB.push_back(wsi);
		}
		*/
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

	struct timeval now= {0,0};
	gettimeofday(&now,NULL);
	static struct timeval pre = {now.tv_sec,now.tv_usec};
	
	unsigned int mlisec = (now.tv_sec - pre.tv_sec) * 1000 + (now.tv_usec - pre.tv_usec)/1000;
	if(mlisec > 100)
	{
		pre.tv_sec = now.tv_sec;
		pre.tv_usec = now.tv_usec;
		responseWS(json);
	}
	else
	{
		DUMP_VAR(mlisec);
	}

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


#if 0
static map<string,int> globalSocket;
static list<string> globalList;
#endif

void writeR2RSocket(const string msg,const string ip)
{
	DUMP_VAR(ip);
#if 0
	auto soketIt = globalSocket.find(ip);
	if(soketIt != globalSocket.end())
	{
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		if(setsockopt (soketIt->second, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
		{
	    	close(soketIt->second);
			globalSocket.erase(soketIt);
			return;
		}
		DUMP_VAR(soketIt->second);
		::write(soketIt->second,msg.c_str(),msg.size());
		return ;
	}
#endif
	
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
	
	DUMP_VAR(ip);
	int ret = inet_pton(AF_INET,ip.c_str(),&(servaddr.sin_addr));
	if(0 > ret)
	{
		_PERROR("inet_pton");
		return;
	}
	DUMP_VAR(sockfd);
    ret = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if(0 > ret)
	{
		_PERROR("Connect::");
		_PERROR("setsockopt failed\n");
		return;
	}
	
	DUMP_VAR(ip);
	struct timeval timeout;      
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	if(setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
	{
	
    	close(sockfd);
		return;
	}
	
	
	DUMP_VAR(sockfd);
    ::write(sockfd,msg.c_str(),msg.size());
	DUMP_VAR(sockfd);

#if 0
	globalSocket[ip] = sockfd;
	globalList.push_back(ip);
	while(globalList.size() > 10)
	{
		auto ipAdress = globalList.front();
		globalList.pop_front();
		auto oldIt = globalSocket.find(ipAdress);
		if(oldIt != globalSocket.end())
		{
	    	close(oldIt->second);
			globalSocket.erase(oldIt);
		}
	}
#endif
}



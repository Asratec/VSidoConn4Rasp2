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

#include <cstring>

#include "debug.h"



#include "tcpcomm.hpp"
#include "wscomm.hpp"

#include "debug.h"



void doTCPSocket()
{
	char str[100];
	int listen_fd;
	struct sockaddr_in servaddr;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 >= listen_fd)
	{
        DUMP_VAR(listen_fd);
		perror("socket");
		::exit(-1);
	}
    DUMP_VAR(listen_fd);
	
	
	memset( &servaddr,0x0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(22100);
	int ret = bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if(0 > ret)
	{
        DUMP_VAR(ret);
		perror("bind");
		::exit(-1);
	}
    DUMP_VAR(listen_fd);
	listen(listen_fd, 10);
    DUMP_VAR(listen_fd);
	if(0 > ret)
	{
        DUMP_VAR(ret);
		perror("listen");
		::exit(-1);
	}
    DUMP_VAR(listen_fd);

	
    while(true)
    {
		int comm_fd = 0;
        DUMP_VAR(comm_fd);
		comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        DUMP_VAR(comm_fd);
    	char data[1024*1024] = {0};
        int size = read(comm_fd,&data,sizeof(data));
    	close(comm_fd);
        DUMP_VAR(size);
        if(0 < size)
        {
        	DUMP_VAR(data);
			notifyR2R(string(data,size));
        }
        else
        {
            perror("read");
            break;
        }
	}
	close(listen_fd);
}




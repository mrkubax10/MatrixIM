// MatrixIM
// Copyright (C) 2021 mrkubax10

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if defined(__unix__)
#include "network/socket.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include "utils/log.h"
bool Socket_connect(Socket* sock,char* address,int port){
    if(sock->connected){
        log_warning("Socket","Socket already connected");
        return false;
    }
    sock->port=port;
    sock->sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sock->sockfd<0){
        log_warning("Socket","Failed to open socket");
        return false;
    }
    sock->serverInfo=gethostbyname(address);
    if(!sock->serverInfo){
        log_warning("Socket","Failed to resolve host %s",address);
        return false;
    }
    bzero((char*)&sock->address,sizeof(sock->address));
    sock->address.sin_family=AF_INET;
    bcopy((char*)sock->serverInfo->h_addr,(char*)&sock->address.sin_addr.s_addr,sock->serverInfo->h_length);
    int retries=1;
    sock->address.sin_port=htons(port);
    setsockopt(sock->sockfd,IPPROTO_TCP,TCP_SYNCNT,&retries,sizeof(retries));
    if(connect(sock->sockfd,(struct sockaddr*)&sock->address,sizeof(sock->address))<0){
        log_warning("Socket","Failed to connect to host %s",address);
        return false;
    }
    sock->connected=true;
    return true;
}
void Socket_send(Socket* sock,char* data,int len){
    if(!sock->connected){
        log_warning("Socket","Failed to send data: socket not connected");
        return;
    }
    int n=write(sock->sockfd,data,len);
    if(n<0)
        log_warning("Socket","Failed to send data: error writing");
}
int Socket_read(Socket* sock,char* dest,int maxlen){
    if(!sock->connected){
        log_warning("Socket","Failed to read data: socket not connected");
        return 0;
    }
    bzero(dest,maxlen);
    int n=read(sock->sockfd,dest,maxlen);
    if(n<0){
        log_warning("Socket","Failed to read from socket: error reading");
        return n;
    }
    return n;
}
char* Socket_readAll(Socket* sock,int* count){
    // FIXME: make this function actually work
    if(!sock->connected){
        log_warning("Socket","Failed to read data: socket not connected");
        *count=0;
        return 0;
    }
    char buf[512];
    char* output=(char*)malloc(0);
    int total=0,n=0,offset=0;
    while(true){
        n=Socket_read(sock,buf,512);
        if(n>total-offset){
            total+=n;
            output=(char*)realloc(output,total);
            memcpy(output+offset,buf,n);
            offset+=n;
        }
    }
    *count=total;
    return output;
}
bool Socket_isConnected(Socket* sock){
    return sock->connected;
}
void Socket_setBlocking(Socket* sock,bool blocking){
    int flags=fcntl(sock->sockfd,F_GETFL,0);
    if(flags==-1)
        return;
    flags=blocking?(flags & ~O_NONBLOCK):(flags|O_NONBLOCK);
    fcntl(sock->sockfd,F_SETFL,flags);
}
void Socket_disconnect(Socket* sock){
    sock->connected=false;
    close(sock->sockfd);
}
void Socket_close(Socket* sock){
    if(!sock->connected){
        free(sock);
        return;
    }
    sock->connected=false;
    close(sock->sockfd);
    free(sock);
}
#endif

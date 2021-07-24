#if defined(__unix__)
#include "network/socket.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>
bool Socket_connect(Socket* sock,char* address,int port){
    if(sock->connected){
        printf("(Warn) [Socket] Socket already connected\n");
        return false;
    }
    sock->port=port;
    sock->sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sock->sockfd<0){
        printf("(Warn) [Socket] Failed to open socket\n");
        return false;
    }
    sock->serverInfo=gethostbyname(address);
    if(!sock->serverInfo){
        printf("(Warn) [Socket] Failed to resolve host %s\n",address);
        return false;
    }
    bzero((char*)&sock->address,sizeof(sock->address));
    sock->address.sin_family=AF_INET;
    bcopy((char*)sock->serverInfo->h_addr,(char*)&sock->address.sin_addr.s_addr,sock->serverInfo->h_length);
    int retries=1;
    sock->address.sin_port=htons(port);
    setsockopt(sock->sockfd,IPPROTO_TCP,TCP_SYNCNT,&retries,sizeof(retries));
    if(connect(sock->sockfd,(struct sockaddr*)&sock->address,sizeof(sock->address))<0){
        printf("(Warn) [Socket] Failed to connect to host %s\n",address);
        return false;
    }
    sock->connected=true;
    return true;
}
void Socket_send(Socket* sock,char* data,int len){
    if(!sock->connected){
        printf("(Warn) [Socket] Failed to send data: socket not connected\n");
        return;
    }
    int n=write(sock->sockfd,data,len);
    if(n<0)
        printf("(Warn) [Socket] Failed to send data: error writing\n");
}
int Socket_read(Socket* sock,char* dest,int maxlen){
    if(!sock->connected){
        printf("(Warn) [Socket] Failed to read data: socket not connected\n");
        return 0;
    }
    bzero(dest,maxlen);
    int n=read(sock->sockfd,dest,maxlen);
    if(n<0){
        printf("(Warn) [Socket] Failed to read from socket: error reading\n");
        return n;
    }
    return n;
}
char* Socket_readAll(Socket* sock,int* count){
    // FIXME: make this function actually work
    if(!sock->connected){
        printf("(Warn) [Socket] Failed to read data: socket not connected\n");
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
void Socket_close(Socket* sock){
    sock->connected=false;
    close(sock->sockfd);
    free(sock);
}
#endif
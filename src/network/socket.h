#ifndef SRC_NETWORK_SOCKET_H
#define SRC_NETWORK_SOCKET_H
#include <stdio.h>
#include <stdbool.h>
#if defined(__unix__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
typedef struct Socket{
    int sockfd,port;
    struct sockaddr_in address;
    struct hostent* serverInfo;
    bool connected;
} Socket;
#endif

Socket* Socket_new();
bool Socket_connect(Socket* sock,char* address,int port);
void Socket_send(Socket* sock,char* data,int len);
int Socket_read(Socket* sock,char* dest,int maxlen);
char* Socket_readAll(Socket* sock,int* count);
bool Socket_isConnected(Socket* sock);
void Socket_setBlocking(Socket* sock,bool blocking);
void Socket_disconnect(Socket* sock);
void Socket_close(Socket* sock);
#endif

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

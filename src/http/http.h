#ifndef SRC_HTTP_HTTP_H
#define SRC_HTTP_HTTP_H
#include "network/socket.h"
extern const char* HTTP_VERSION;
typedef struct HTTPResponseInfo{
    char* version;
    int code;
    char* datatype;
    char* data;
} HTTPResponseInfo;
HTTPResponseInfo* HTTPResponseInfo_new();
int http_findEntry(char** lines,char* entry);
char* http_getValue(char** lines,char* entry);
char* http_getValueFromIndex(char** lines,int index);
int http_findDataOffset(char* data);
HTTPResponseInfo* http_parseResponse(char* response);
void http_sendGETRequest(char* path,Socket* sock);

#endif
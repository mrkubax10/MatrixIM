#ifndef SRC_HTTP_HTTP_H
#define SRC_HTTP_HTTP_H
#include "network/socket.h"
extern const char* HTTP_VERSION;
#define HTTP_CODE_OK 200
#define HTTP_CODE_NOT_FOUND 404
#define HTTP_CODE_BAD_REQUEST 400
#define HTTP_CODE_UNAUTHORIZED 401
typedef struct HTTPResponseInfo{
    char* version;
    int code;
    char* datatype;
    char* data;
} HTTPResponseInfo;
HTTPResponseInfo* HTTPResponseInfo_new();
void HTTPResponseInfo_destroy(HTTPResponseInfo* info);
int http_findEntry(char** lines,char* entry);
char* http_getValue(char** lines,char* entry);
char* http_getValueFromIndex(char** lines,int index);
int http_findDataOffset(char* data);
HTTPResponseInfo* http_parseResponse(char* response);
void http_sendGETRequest(char* path,char* host,Socket* sock,char* accessToken);
void http_sendPOSTRequest(char* path,char* host,char* datatype,int datalength,char* data,char* accessToken,Socket* sock);
void http_sendPUTRequest(char* path,char* host,char* datatype,int dataLength,char* data,char* accessToken,Socket* sock);
HTTPResponseInfo* http_readResponse(Socket* sock);
char* http_toHTTPURL(char* url);
#endif
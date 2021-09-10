#include "http/http.h"
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include <stdio.h>
const char* HTTP_VERSION="HTTP/1.1";
HTTPResponseInfo* HTTPResponseInfo_new(){
    HTTPResponseInfo* output=(HTTPResponseInfo*)malloc(sizeof(HTTPResponseInfo));
    return output;
}
void HTTPResponseInfo_destroy(HTTPResponseInfo* info){
    free(info->version);
    free(info->datatype);
    free(info->data);
    free(info);
}
int http_findEntry(char** lines,char* entry){
    int index=0;
    int dataLength=0;
    while(lines[index]){
        char** data=split(lines[index],':',&dataLength);
        if(strcmp(data[0],"\r\n")==0){
            array_free((void**)data,dataLength);
            return -1;
        }
        if(data[0]){
            if(strcmp(data[0],entry)==0){
                array_free((void**)data,dataLength);
                return index;
            }
        }
        array_free((void**)data,dataLength);
        index++;
    }
    return -1;
}
char* http_getValue(char** lines,char* entry){
    int index=http_findEntry(lines,entry);
    if(index==-1)
        return 0;
    return http_getValueFromIndex(lines,index);
}
char* http_getValueFromIndex(char** lines,int index){
    bool startReading=false;
    char* data=(char*)malloc(0);
    int dataCount=0;
    int dataOffset=0;
    for(int i=0; i<strlen(lines[index])-dataOffset; i++){
        if(startReading){
            dataCount++;
            data=(char*)realloc(data,dataCount);
            data[dataCount-1]=lines[index][i+dataOffset];
        }
        if(lines[index][i]==':' && !startReading){
            if(lines[index][i+1]){
                if(lines[index][i+1]==' ')
                    dataOffset=1;
            }
            startReading=true;
        }
    }
    data=(char*)realloc(data,dataCount+1);
    data[dataCount]=0;
    return data;
}
int http_findDataOffset(char* data){
    int index=0;
    while(data[index]){
        if(data[index+1] && data[index+2] && data[index+3]){
            if(data[index]=='\r' && data[index+1]=='\n' && data[index+2]=='\r' && data[index+3]=='\n')
            return index+2;
        }
        index++;
    }
    return -1;
}
HTTPResponseInfo* http_parseResponse(char* response){
    HTTPResponseInfo* output=HTTPResponseInfo_new();
    int linesCount=0;
    int headerDataCount=0;
    char** lines=splitByString(response,"\r\n",&linesCount);
    char** header=split(lines[0],' ',&headerDataCount);
    if(strcmp(header[0],HTTP_VERSION)!=0)
        printf("(Warn) [HTTP] Received response with different HTTP version than 1.1\n");
    output->version=(char*)malloc(strlen(header[0])+1);
    memcpy(output->version,header[0],strlen(header[0]));
    output->version[strlen(header[0])]=0;
    output->code=atoi(header[1]);
    array_free((void**)header,headerDataCount);
    int contentTypeIndex=http_findEntry(lines,"Content-Type");
    if(contentTypeIndex!=-1){
        char* value=http_getValueFromIndex(lines,contentTypeIndex);
        output->datatype=(char*)malloc(strlen(value)+1);
        memcpy(output->datatype,value,strlen(value));
        output->datatype[strlen(value)]=0;
        free(value);
    }
    else{
        output->datatype=(char*)malloc(1);
        output->datatype[0]=0;
    }
    int dataOffset=http_findDataOffset(response);
    if(dataOffset!=-1){
        output->data=(char*)malloc(strlen(response)-dataOffset+1);
        memcpy(output->data,response+dataOffset,strlen(response)-dataOffset);
        output->data[strlen(response)-dataOffset]=0;
    }
    else{
        output->data=(char*)malloc(1);
        output->data[0]=0;
    }
    array_free((void**)lines,linesCount);
    return output;
}
void http_sendGETRequest(char* path,char* host,Socket* sock,char* accessToken){
    int length;
    char* request;
    if(accessToken!=0){
        length=snprintf(0,0,"GET %s %s\r\nHost: %s\r\nAuthorization: Bearer %s\r\n\r\n",path,HTTP_VERSION,host,accessToken);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"GET %s %s\r\nHost: %s\r\nAuthorization: Bearer %s\r\n\r\n",path,HTTP_VERSION,host,accessToken);
    }
    else{
        length=snprintf(0,0,"GET %s %s\r\nHost: %s\r\n\r\n",path,HTTP_VERSION,host);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"GET %s %s\r\nHost: %s\r\n\r\n",path,HTTP_VERSION,host);
    }
    Socket_send(sock,request,length);
    free(request);
}
void http_sendPOSTRequest(char* path,char* host,char* datatype,int datalength,char* data,char* accessToken,Socket* sock){
    char* request;
    int length;
    if(accessToken!=0){
        length=snprintf(0,0,"POST %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Bearer %s\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,accessToken,host,data);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"POST %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Bearer %s\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,accessToken,host,data);
    }
    else{
        length=snprintf(0,0,"POST %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,host,data);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"POST %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,host,data);
    }
    Socket_send(sock,request,length);
    free(request);
}
void http_sendPUTRequest(char* path,char* host,char* datatype,int datalength,char* data,char* accessToken,Socket* sock){
    char* request;
    int length;
    if(accessToken!=0){
        length=snprintf(0,0,"PUT %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Bearer %s\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,accessToken,host,data);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"PUT %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nAuthorization: Bearer %s\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,accessToken,host,data);
    }
    else{
        length=snprintf(0,0,"PUT %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,host,data);
        request=(char*)malloc(length+1);
        snprintf(request,length+1,"PUT %s %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s",path,HTTP_VERSION,datatype,datalength,host,data);
    }
    Socket_send(sock,request,length);
    free(request);
}
HTTPResponseInfo* http_readResponse(Socket* sock){
    char data[4096];
    Socket_read(sock,data,4096);
    HTTPResponseInfo* response=http_parseResponse(data);
    return response;
}
char* http_toHTTPURL(char* url){
    int length=0;
    for(int i=0; i<strlen(url); i++){
        if(url[i]=='!' || url[i]==':')
            length+=3;
        else
            length++;
    }
    char* output=malloc(length+1);
    int index=0;
    for(int i=0; i<strlen(url); i++){
        if(url[i]=='!'){
            output[index]='%';
            output[index+1]='2';
            output[index+2]='1';
            index+=3;
        }
        else if(url[i]==':'){
            output[index]='%';
            output[index+1]='3';
            output[index+2]='A';
            index+=3;
        }
        else{
            output[index]=url[i];
            index++;
        }
    }
    output[length]=0;
    return output;
}
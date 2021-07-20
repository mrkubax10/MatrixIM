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
    char** lines=split(response,'\n',&linesCount);
    char** header=split(lines[0],' ',&headerDataCount);
    if(strcmp(header[0],HTTP_VERSION)!=0)
        printf("(Warn) [HTTP] Received response with different version (%s)\n",header[0]);
    output->version=(char*)malloc(strlen(header[0])+1);
    strcpy(output->version,header[0]);
    output->code=atoi(header[1]);
    array_free((void**)header,headerDataCount);
    int contentTypeIndex=http_findEntry(lines,"Content-Type");
    if(contentTypeIndex!=-1){
        char* value=http_getValueFromIndex(lines,contentTypeIndex);
        output->datatype=(char*)malloc(strlen(value));
        strcpy(output->datatype,value);
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
void http_sendGETRequest(char* path,Socket* sock){
    int resultLength=strlen("GET ")+strlen(path)+strlen(HTTP_VERSION)+6;
    char* request=(char*)malloc(resultLength);
    snprintf(request,resultLength,"GET %s %s\r\n\r\n",path,HTTP_VERSION);
    request[resultLength]=0;
    Socket_send(sock,request,resultLength);
    free(request);
}
void http_sendPOSTRequest(char* path,char* datatype,char* data,Socket* sock){
    int resultLength=strlen("POST ")+strlen(path)+strlen(HTTP_VERSION)+2;
    resultLength+=strlen("Content-Type: ")+strlen(datatype)+3;
    resultLength+=strlen(data)+1;
    char* request=(char*)malloc(resultLength);
    snprintf(request,resultLength,"POST %s %s\nContent-Type: %s\n\r\n%s",path,HTTP_VERSION,datatype,data);
    request[resultLength]=0;
    Socket_send(sock,request,resultLength);
    free(request);
}
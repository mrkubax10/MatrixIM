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
    while(lines[index]){
        char** data=split(lines[index],':');
        if(strcmp(data[0],"\r\n")==0){
            free(data);
            return -1;
        }
        if(data[0]){
            if(strcmp(data[0],entry)==0){
                free(data);
                return index;
            }
        }
        free(data);
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
        if(data[index+1]){
            if(data[index]=='\r' && data[index+1]=='\n')
            return index+2;
        }
        index++;
    }
    return -1;
}
HTTPResponseInfo* http_parseResponse(char* response){
    HTTPResponseInfo* output=HTTPResponseInfo_new();
    char** lines=split(response,'\n');
    char** header=split(lines[0],' ');
    if(strcmp(header[0],HTTP_VERSION)!=0)
        printf("(Warn) [HTTP] Received response with different version (%s)\n",header[0]);
    output->version=(char*)malloc(strlen(header[0])+1);
    strcpy(output->version,header[0]);
    output->code=atoi(header[1]);
    free(header);
    int contentTypeIndex=http_findEntry(lines,"Content-Type");
    if(contentTypeIndex!=-1){
        char* value=http_getValueFromIndex(lines,contentTypeIndex);
        output->datatype=(char*)malloc(strlen(value));
        strcpy(output->datatype,value);
        if(strcmp(value,"application/json")==0){
            int dataOffset=http_findDataOffset(response);
            if(dataOffset!=-1){
                output->data=(char*)malloc(strlen(response)-dataOffset);
                memcpy(output->data,response+dataOffset,strlen(response)-dataOffset);
                free(value);
                free(lines);
                return output;
            }
        }
        free(value);
    }
    free(lines);
    return 0;
}
void http_sendGETRequest(char* path,Socket* sock){
    int resultLength=strlen("GET ")+strlen(path)+1+strlen(HTTP_VERSION)+1;
    char* request=(char*)malloc(resultLength);
    snprintf(request,resultLength,"GET %s %s\r\n",path,HTTP_VERSION);
    Socket_send(sock,request,resultLength);
    free(request);
}
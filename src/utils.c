#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
char** split(char* str,char ch,int* len){
    char** output=0;
    int elements=0,tempLength=0;
    bool hasData=false;
    char* temp=0;
    for(int i=0; i<strlen(str); i++){
        if(str[i]==ch){
            if(!hasData)
                continue;
            elements++;
            output=realloc(output,elements*sizeof(char*));
            output[elements-1]=malloc(tempLength+1);
            memcpy(output[elements-1],temp,tempLength);
            output[elements-1][tempLength]=0;
            free(temp);
            tempLength=0;
            temp=0;
            hasData=false;
        }
        else{
            tempLength++;
            temp=realloc(temp,tempLength*sizeof(char));
            temp[tempLength-1]=str[i];
            hasData=true;
        }
    }
    if(tempLength>0){
        elements++;
        output=realloc(output,elements*sizeof(char*));
        output[elements-1]=malloc(strlen(temp)+1);
        memcpy(output[elements-1],temp,tempLength);
        output[elements-1][tempLength]=0;
        free(temp);
    }
    *len=elements;
    return output;
}
char** splitByString(char* str,char* str2,int* len){
    char** output=0;
    if(strlen(str2)>=strlen(str)){
        *len=1;
        output=realloc(output,1*sizeof(char*));
        output[0]=malloc(strlen(str)+1);
        strcpy(output[0],str);
        return output;
    }
    int elements=0,tempLength=0;
    bool hasData=false;
    char* temp=0;
    for(int i=0; i<strlen(str); i++){
        if(strlen(str2)+i-1<strlen(str) && memcmp((void*)(str+i),(void*)str2,strlen(str2))==0){
            if(!hasData)
                continue;
            elements++;
            output=realloc(output,elements*sizeof(char*));
            output[elements-1]=malloc(tempLength+1);
            memcpy(output[elements-1],temp,tempLength);
            output[elements-1][tempLength]=0;
            free(temp);
            temp=0;
            tempLength=0;
            hasData=false;
            i+=strlen(str2)-1;
        }
        else{
            tempLength++;
            temp=realloc(temp,tempLength*sizeof(char));
            temp[tempLength-1]=str[i];
            hasData=true;
        }
    }
    if(tempLength>0){
        elements++;
        output=realloc(output,elements*sizeof(char*));
        output[elements-1]=malloc(tempLength+1);
        memcpy(output[elements-1],temp,tempLength);
        output[elements-1][tempLength]=0;
        free(temp);
    }
    *len=elements;
    return output;
}
void array_free(void** data,int len){
    for(int i=0; i<len; i++){
       free(data[i]);
    }
    free(data);
}

char* intToString(int num){
    int length=snprintf(0,0,"%d",num);
    char* output=(char*)malloc(length+1);
    snprintf(output,length+1,"%d",num);
    return output;
}
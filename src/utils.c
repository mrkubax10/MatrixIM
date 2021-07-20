#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
char** split(char* str,char ch,int* len){
    char** output=(char**)malloc(0);
    char* tempString=(char*)malloc(0);
    int elements=0,index=0;
    bool hasData=false;
    for(int i=0; i<strlen(str); i++){
        if(str[i]==ch){
            if(!hasData)
                continue;
            elements++;
            output=(char**)realloc(output,elements*sizeof(char*));
            output[elements-1]=(char*)malloc(index+2);
            tempString=(char*)realloc(tempString,index+2);
            tempString[index+1]=0;
            strcpy(output[elements-1],tempString);
            free(tempString);
            tempString=(char*)malloc(0);
            hasData=false;
            index=0;
        }
        else{
            hasData=true;
            tempString=(char*)realloc(tempString,index+1);
            tempString[index]=str[i];
            index++;
        }
    }
    if(hasData){
       output=(char**)realloc(output,(elements+1)*sizeof(char*));
       output[elements]=(char*)malloc(index+2);
       tempString=(char*)realloc(tempString,index+2);
       tempString[index+1]=0;
       strcpy(output[elements],tempString);
       free(tempString);
       elements++;
    }
    if(len)
        *len=elements;
    return output;
}
void array_free(void** data,int len){
    for(int i=0; i<len; i++){
       free(data[i]);
    }
    free(data);
}
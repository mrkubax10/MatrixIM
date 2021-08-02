#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#if defined(__linux__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif
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

#if defined(__linux__)
char* getConfigDir(){
    char* home=getenv("HOME");
    if(strcmp(home,"")==0){
        printf("(Warn) [Get Config Dir] Failed to detect config directory, application directory will be used instead\n");
        return "";
    }
    char* output=strcat(home,"/.config/matrixim");
    return output;
}
void createFolder(char* path){
    if(!folderExists(path))
        mkdir(path,0700);
}
bool folderExists(char* path){
    struct stat s={0};
    if(!stat(path,&s))
        return s.st_mode & S_IFDIR;
    else
        return false;
}
#elif defined(_WIN32)
char* getConfigDir(){
    char* appdata=getenv("appdata");
    if(strcmp(appdata,"")==0){
        printf("(Warn) [Get Config Dir] Failed to detect config directory, application directory will be used instead\n");
        return "";
    }
    char* output=strcat(appdata,"/matrixim");
    return output;
}
void createFolder(char* path){
    if(!folderExists(path))
        CreateDirectory(path,0);
}
#else
char* getConfigDir(){
    printf("(Warn) [Get Config Dir] Unsupported operating system, application directory will be used instead\n");
    return "";
}
void createFolder(char* path){
    printf("(Err) [Create Folder] Unsupported operating system, exiting!");
    exit(1);
}
#endif
char* intToString(int num){
    int length=snprintf(0,0,"%d",num);
    char* output=(char*)malloc(length+1);
    snprintf(output,length+1,"%d",num);
    return output;
}
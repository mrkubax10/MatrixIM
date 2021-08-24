#include "utils/filesystem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__linux__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

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
char* loadFullFile(char* path){
    FILE* file=fopen(path,"r");
    if(!file)
        return 0;
    fseek(file,0,SEEK_END);
    int fileLength=ftell(file);
    rewind(file);
    char* fileData=malloc(fileLength+1);
    fread(fileData,1,fileLength,file);
    fclose(file);
    fileData[fileLength]=0;
    return fileData;
}
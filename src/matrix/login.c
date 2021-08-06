#include "matrix/login.h"
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
LoginInfo* LoginInfo_new(){
    LoginInfo* output=(LoginInfo*)malloc(sizeof(LoginInfo));
    return output;
}
char* matrix_createPasswordLoginRequest(char* username,char* password,char* deviceName,char* deviceID){
    if(deviceID){
        int length=snprintf(0,0,"{\n\t\"type\": \"m.login.password\",\n\t\"identifier\": {\n\t\t\"type\": \"m.id.user\",\n\t\t\"user\": \"%s\"\n\t},\n\t\"password\": \"%s\",\n\t\"device_id\": \"%s\"\n}",username,password,deviceID);
        char* output=(char*)malloc(length+1);
        snprintf(output,length+1,"{\n\t\"type\": \"m.login.password\",\n\t\"identifier\": {\n\t\t\"type\": \"m.id.user\",\n\t\t\"user\": \"%s\"\n\t},\n\t\"password\": \"%s\",\n\t\"device_id\": \"%s\"\n}",username,password,deviceID);
        return output;
    }
    else{
        int length=snprintf(0,0,"{\n\t\"type\": \"m.login.password\",\n\t\"identifier\": {\n\t\t\"type\": \"m.id.user\",\n\t\t\"user\": \"%s\"\n\t},\n\t\"password\": \"%s\",\n\t\"initial_device_display_name\": \"%s\"\n}",username,password,deviceName);
        char* output=(char*)malloc(length+1);
        snprintf(output,length+1,"{\n\t\"type\": \"m.login.password\",\n\t\"identifier\": {\n\t\t\"type\": \"m.id.user\",\n\t\t\"user\": \"%s\"\n\t},\n\t\"password\": \"%s\",\n\t\"initial_device_display_name\": \"%s\"\n}",username,password,deviceName);
        return output;
    }
}
char* matrix_createLogoutRequest(char* accessToken){
    cJSON* root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"access_token",accessToken);
    char* output=cJSON_Print(root);
    cJSON_free((void*)root);
    return output;
}
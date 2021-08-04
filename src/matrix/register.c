#include "matrix/register.h"
#include <cjson/cJSON.h>
char* matrix_createPasswordRegisterRequest(char* username,char* password,char* deviceName,char* deviceID,char* session){
    cJSON* root=cJSON_CreateObject();
    cJSON* auth=cJSON_CreateObject();
    if(session){
        cJSON_AddStringToObject(auth,"session",session);
        cJSON_AddStringToObject(auth,"type","m.login.dummy");
        cJSON_AddItemToObject(root,"auth",auth);
    }
    cJSON_AddStringToObject(root,"username",username);
    cJSON_AddStringToObject(root,"password",password);
    if(!deviceID){
        cJSON_AddStringToObject(root,"device_id",deviceID);
        cJSON_AddStringToObject(root,"initial_device_display_name",deviceName);
    }
    char* output=cJSON_Print(root);
    cJSON_free((void*)auth);
    cJSON_free((void*)root);
    return output;
}
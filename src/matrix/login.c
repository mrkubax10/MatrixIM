#include "matrix/login.h"
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "utils.h"
#include "app.h"
#include "http/http.h"
#include "utils/message.h"
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
char* matrix_generateTnxID(){
    int num=rand();
    return intToString(num);
}
bool matrix_loginPassword(char* ip,char* username,char* password,char* deviceName,char* deviceID){
    char responseData[4096];
    char* loginRequest;
    if(deviceID!=0)
        loginRequest=matrix_createPasswordLoginRequest(username,password,0,deviceID);
    else
        loginRequest=matrix_createPasswordLoginRequest(username,password,"MatrixIM",0);
    http_sendPOSTRequest("/_matrix/client/r0/login",ip,"application/json",strlen(loginRequest),loginRequest,0,app->homeserverSocket);
    free(loginRequest);
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    cJSON* jsonData=cJSON_Parse(response->data);
    if(response->code!=HTTP_CODE_OK){
        if(strcmp(response->datatype,"application/json")==0){
            const cJSON* jsonErrcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
            if(jsonErrcode){
                if(strcmp(jsonErrcode->valuestring,"M_UNRECOGNIZED")==0)
                    showErrorDialog("Server did not recognize login type");
                else if(strcmp(jsonErrcode->valuestring,"M_FORBIDDEN")==0)
                    showErrorDialog("Authentication data is incorrect");
                HTTPResponseInfo_destroy(response);
                cJSON_free((void*)jsonErrcode);
                cJSON_free((void*)jsonData);
                return false;
            }
            else{
                HTTPResponseInfo_destroy(response);
                cJSON_free((void*)jsonErrcode);
                cJSON_free((void*)jsonData);
                showErrorDialog("Unknown error");
                return false;
            }
        }
        else{
            HTTPResponseInfo_destroy(response);
            cJSON_free((void*)jsonData);
            showErrorDialog("Server responded with unexpected data type");
            return false;
        }
    }
    const cJSON* jsonUserID=cJSON_GetObjectItemCaseSensitive(jsonData,"user_id");
    const cJSON* jsonAccessToken=cJSON_GetObjectItemCaseSensitive(jsonData,"access_token");
    const cJSON* jsonDeviceID=cJSON_GetObjectItemCaseSensitive(jsonData,"device_id");
    if(!jsonUserID || !jsonAccessToken || !jsonDeviceID){
        cJSON_free((void*)jsonUserID);
        cJSON_free((void*)jsonAccessToken);
        cJSON_free((void*)jsonDeviceID);
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(response);
        showErrorDialog("Failed to parse response");
        return false;
    }
    app->loginInfo->userID=(char*)malloc(strlen(jsonUserID->valuestring)+1);
    strcpy(app->loginInfo->userID,jsonUserID->valuestring);

    app->loginInfo->accessToken=(char*)malloc(strlen(jsonAccessToken->valuestring)+1);
    strcpy(app->loginInfo->accessToken,jsonAccessToken->valuestring);
    
    app->loginInfo->deviceID=(char*)malloc(strlen(jsonDeviceID->valuestring)+1);
    strcpy(app->loginInfo->deviceID,jsonDeviceID->valuestring);

    int userIDDataLength=0;
    char** userIDData=split(jsonUserID->valuestring,':',&userIDDataLength);
    if(userIDDataLength!=2){
        HTTPResponseInfo_destroy(response);
        cJSON_free((void*)jsonUserID);
        cJSON_free((void*)jsonAccessToken);
        cJSON_free((void*)jsonDeviceID);
        cJSON_free((void*)jsonData);
        array_free((void**)userIDData,userIDDataLength);
        showErrorDialog("Failed to get homeserver name");
        return false;
    }
    app->loginInfo->homeserverName=(char*)malloc(strlen(userIDData[1])+1);
    strcpy(app->loginInfo->homeserverName,userIDData[1]);
    printf("(Log) [Login] User ID: %s\n",jsonUserID->valuestring);
    array_free((void**)userIDData,userIDDataLength);
    HTTPResponseInfo_destroy(response);
    cJSON_free((void*)jsonUserID);
    cJSON_free((void*)jsonAccessToken);
    cJSON_free((void*)jsonDeviceID);
    cJSON_free((void*)jsonData);
    app->loggedIn=true;
    app->settings->wasLoggedIn=true;
    app->settings->deviceID=app->loginInfo->deviceID;
    app->settings->lastUsername=(char*)malloc(strlen(username)+1);
    strcpy(app->settings->lastUsername,username);
    app->settings->lastPassword=(char*)malloc(strlen(password)+1);
    strcpy(app->settings->lastPassword,password);
    return true;
}
// MatrixIM
// Copyright (C) 2021 mrkubax10

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "matrix/register.h"

#include <cjson/cJSON.h>

#include "http/http.h"
#include "app.h"
#include "utils/message.h"
#include "utils/array.h"
#include "utils/str.h"
#include "utils/log.h"
#include "translation/translation.h"
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
bool matrix_registerPassword(char* ip,char* username,char* password){
    char responseData[4096];
    char* availableUsernameRequest=(char*)malloc(strlen("/_matrix/client/r0/register/available?username=")+1);
    strcpy(availableUsernameRequest,"/_matrix/client/r0/register/available?username=");
    strcat(availableUsernameRequest,username);
    http_sendGETRequest(availableUsernameRequest,ip,app->homeserverSocket,0);
    free(availableUsernameRequest);
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* responseInfo=http_parseResponse(responseData);
    if(strcmp(responseInfo->datatype,"application/json")!=0){
        showErrorDialog(_("Server responded with unexpected data type"));
        HTTPResponseInfo_destroy(responseInfo);
        return false;
    }
    cJSON* jsonData=cJSON_Parse(responseInfo->data);
    if(!jsonData){
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response"));
        return false;
    }
    cJSON* errcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
    if(cJSON_IsString(errcode)){
        if(strcmp(errcode->valuestring,"M_USER_IN_USE")==0)
            showErrorDialog(_("Username already taken"));
        else
            showErrorDialog(_("Unknown error"));
        cJSON_free((void*)errcode);
        cJSON_free((void*)jsonData);
        return false;
    }
    cJSON* available=cJSON_GetObjectItemCaseSensitive(jsonData,"available");
    if(!cJSON_IsBool(available)){
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response: available attribute is not bool"));
        return false;
    }
    if(!available->valueint){
        cJSON_free((void*)jsonData);
        cJSON_free((void*)available);
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Username is already taken or it is invalid"));
        return false;
    }
    cJSON_free((void*)available);
    cJSON_free((void*)jsonData);
    HTTPResponseInfo_destroy(responseInfo);
    char* registerRequest;
    if(app->settings->deviceID!=0)
        registerRequest=matrix_createPasswordRegisterRequest(username,password,0,app->settings->deviceID,0);
    else
        registerRequest=matrix_createPasswordRegisterRequest(username,password,"MatrixIM",0,0);
    http_sendPOSTRequest("/_matrix/client/r0/register",ip,"application/json",strlen(registerRequest),registerRequest,0,app->homeserverSocket);
    Socket_read(app->homeserverSocket,responseData,4096);
    responseInfo=http_parseResponse(responseData);
    if(strcmp(responseInfo->datatype,"application/json")!=0){
        showErrorDialog(_("Server responded with unexpected data type"));
        HTTPResponseInfo_destroy(responseInfo);
        return false;
    }
    jsonData=cJSON_Parse(responseInfo->data);
    if(!jsonData){
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response"));
        return false;
    }
    if(responseInfo->code!=HTTP_CODE_UNAUTHORIZED){
        cJSON* errcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
        if(!cJSON_IsString(errcode)){
            cJSON_free((void*)jsonData);
            HTTPResponseInfo_destroy(responseInfo);
            showErrorDialog(_("Unknown error"));
            return false;
        }
        if(strcmp(errcode->valuestring,"M_WEAK_PASSWORD")==0){ // Password was too weak
            cJSON_free((void*)jsonData);
            cJSON_free((void*)errcode);
            HTTPResponseInfo_destroy(responseInfo);
            showErrorDialog(_("Password is too weak"));
            return false;
        }
        else if(strcmp(errcode->valuestring,"M_UNKNOWN")==0){ // Server didn't recognize provided auth method
            cJSON_free((void*)jsonData);
            cJSON_free((void*)errcode);
            HTTPResponseInfo_destroy(responseInfo);
            showErrorDialog(_("Invalid authentication method"));
            return false;
        }
        cJSON_free((void*)jsonData);
        cJSON_free((void*)errcode);
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Unknown error"));
        return false;
    }
    cJSON* session=cJSON_GetObjectItemCaseSensitive(jsonData,"session");
    if(!cJSON_IsString(session)){
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response: session attribute is not string"));
        return false;
    }
    if(app->settings->deviceID!=0)
        registerRequest=matrix_createPasswordRegisterRequest(username,password,0,app->settings->deviceID,session->valuestring);
    else
        registerRequest=matrix_createPasswordRegisterRequest(username,password,"MatrixIM",0,session->valuestring);
    HTTPResponseInfo_destroy(responseInfo);
    cJSON_free((void*)session);
    cJSON_free((void*)jsonData);
    http_sendPOSTRequest("/_matrix/client/r0/register",ip,"application/json",strlen(registerRequest),registerRequest,0,app->homeserverSocket);
    Socket_read(app->homeserverSocket,responseData,4096);
    responseInfo=http_parseResponse(responseData);
    if(responseInfo->code!=HTTP_CODE_OK){
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Server responded with HTTP code different than 200"));
        return false;
    }
    jsonData=cJSON_Parse(responseInfo->data);
    if(!jsonData){
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response"));
        return false;
    }
    cJSON* jsonAccessToken=cJSON_GetObjectItemCaseSensitive(jsonData,"access_token");
    cJSON* jsonUserID=cJSON_GetObjectItemCaseSensitive(jsonData,"user_id");
    cJSON* jsonDeviceID=cJSON_GetObjectItemCaseSensitive(jsonData,"device_id");
    if(!cJSON_IsString(jsonUserID) || !cJSON_IsString(jsonAccessToken) || !cJSON_IsString(jsonDeviceID)){
        cJSON_free((void*)jsonUserID);
        cJSON_free((void*)jsonAccessToken);
        cJSON_free((void*)jsonDeviceID);
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        showErrorDialog(_("Failed to parse response"));
        return false;
    }
    int userIDDataLength=0;
    char** userIDData=split(jsonUserID->valuestring,':',&userIDDataLength);
    if(userIDDataLength!=2){
        HTTPResponseInfo_destroy(responseInfo);
        cJSON_free((void*)jsonUserID);
        cJSON_free((void*)jsonAccessToken);
        cJSON_free((void*)jsonDeviceID);
        cJSON_free((void*)jsonData);
        array_free((void**)userIDData,userIDDataLength);
        showErrorDialog(_("Failed to get homeserver name"));
        return false;
    }
    app->loginInfo->homeserverName=(char*)malloc(strlen(userIDData[1])+1);
    strcpy(app->loginInfo->homeserverName,userIDData[1]);
    log_info("Register","User ID: %s",jsonUserID->valuestring);
    array_free((void**)userIDData,userIDDataLength);
    HTTPResponseInfo_destroy(responseInfo);
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
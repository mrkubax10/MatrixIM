#include "matrix/room.h"
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "http/http.h"
#include "app.h"
#include "screens/main_screen.h"
#include "utils/message.h"
MatrixRoom* MatrixRoom_new(){
    MatrixRoom* output=(MatrixRoom*)malloc(sizeof(MatrixRoom));
    return output;
}
void MatrixRoom_destroy(MatrixRoom* room){
    free(room);
}
char* matrix_joinRoom(char* room){
    if(!mainScreen)
        return 0;
    int pathLength=snprintf(0,0,"/_matrix/client/r0/directory/room/%s",room);
    char* path=(char*)malloc(pathLength+1);
    // Obtain room ID from room alias
    snprintf(path,pathLength+1,"/_matrix/client/r0/directory/room/%s",room);
    http_sendGETRequest(path,app->loginInfo->homeserverName,app->homeserverSocket,0);
    free(path);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        cJSON* jsonData=cJSON_Parse(response->data);
        char* errorMsg=(char*)malloc(strlen("Failed to enter room")+1);
        strcpy(errorMsg,"Failed to enter room");
        if(jsonData){
            cJSON* error=cJSON_GetObjectItemCaseSensitive(jsonData,"error");
            if(error){
                int length=snprintf(0,0,"Failed to enter room: %s",error->valuestring);
                free(errorMsg);
                errorMsg=(char*)malloc(length+1);
                snprintf(errorMsg,length+1,"Failed to enter room: %s",error->valuestring);
            }
            cJSON_Delete(jsonData);
        }
        showErrorDialog(errorMsg);
        HTTPResponseInfo_destroy(response);
        free(errorMsg);
        return 0;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog("Failed to parse response while entering room");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* roomID=cJSON_GetObjectItemCaseSensitive(jsonData,"room_id");
    if(!roomID){
        showErrorDialog("Failed to parse response while entering room");
        HTTPResponseInfo_destroy(response);
        cJSON_Delete(jsonData);
        return 0;
    }
    HTTPResponseInfo_destroy(response);
    if(!matrix_joinRoomWithID(roomID->valuestring)){
        cJSON_Delete(jsonData);
        return 0;
    }
    char* id=malloc(strlen(roomID->valuestring)+1);
    strcpy(id,roomID->valuestring);
    cJSON_Delete(jsonData);
    return id;
}
bool matrix_joinRoomWithID(char* id){
    for(unsigned int i=0; i<mainScreen->enteredRooms->capacity; i++){
        MatrixRoom* matrixRoom=mainScreen->enteredRooms->data[i];
        if(strcmp(matrixRoom->roomID,id)==0){
            showInfoDialog("You already entered this room");
            return false;
        }
    }
    int pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/join",id);
    char* path=(char*)malloc(pathLength+1);
    snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/join",id);
    http_sendPOSTRequest(path,app->loginInfo->homeserverName,"application/json",0,"",app->loginInfo->accessToken,app->homeserverSocket);
    free(path);
    HTTPResponseInfo* response=http_readResponse(app->homeserverSocket);
    if(response->code!=HTTP_CODE_OK){
        showErrorDialog("Failed to enter room");
        HTTPResponseInfo_destroy(response);
        return false;
    }
    HTTPResponseInfo_destroy(response);
    return true;
}
bool matrix_leaveRoom(char* roomID){
    int pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/leave",roomID);
    char* path=malloc(pathLength+1);
    snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/leave",roomID);
    char* url=http_toHTTPURL(path);
    free(path);
    http_sendPOSTRequest(url,app->loginInfo->homeserverName,"",0,"",app->loginInfo->accessToken,app->homeserverSocket);
    free(url);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        showErrorDialog("Failed to leave room");
        HTTPResponseInfo_destroy(response);
        return false;
    }
    return true;
}
bool matrix_sendMessage(char* msg,char* roomID){
    char* tnxID=matrix_generateTnxID();
    int pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/send/m.room.message/%s",roomID,tnxID);
    char* path=malloc(pathLength+1);
    snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/send/m.room.message/%s",roomID,tnxID);
    cJSON* root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"msg_type","m.text"); // At the moment we only support text messages
    cJSON_AddStringToObject(root,"body",msg);
    char* data=cJSON_Print(root);
    http_sendPUTRequest(path,app->loginInfo->homeserverName,"application/json",strlen(data),data,app->loginInfo->accessToken,app->homeserverSocket);
    cJSON_Delete(root);
    free(path);
    free(tnxID);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        printf("%s\n",response->data);
        HTTPResponseInfo_destroy(response);
        return false;
    }
    HTTPResponseInfo_destroy(response);
    return true;
}
char* matrix_createRoom(char* alias,char* name,char* topic,bool visible){
    cJSON* root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"preset","public_chat");
    cJSON_AddStringToObject(root,"room_alias_name",alias);
    cJSON_AddStringToObject(root,"name",name);
    cJSON_AddStringToObject(root,"topic",topic);
    cJSON_AddBoolToObject(root,"visible",visible);
    cJSON* creationContent=cJSON_CreateObject();
    cJSON_AddBoolToObject(creationContent,"m.federate",false);
    cJSON_AddItemToObject(root,"creation_content",creationContent);
    char* data=cJSON_Print(root);
    http_sendPOSTRequest("/_matrix/client/r0/createRoom",app->loginInfo->homeserverName,"application/json",strlen(data),data,app->loginInfo->accessToken,app->homeserverSocket);
    cJSON_Delete(root);
    HTTPResponseInfo* response=http_readResponse(app->homeserverSocket);
    if(strcmp(response->datatype,"application/json")!=0){
        showErrorDialog("Server responded with unexpected datatype");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    if(response->code!=HTTP_CODE_OK){
        if(response->code==HTTP_CODE_BAD_REQUEST){
            cJSON* jsonData=cJSON_Parse(response->data);
            if(!jsonData){
                showErrorDialog("Failed to parse response while handling error");
                HTTPResponseInfo_destroy(response);
                return 0;
            }
            cJSON* errcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
            if(!errcode){
                showErrorDialog("Failed to parse response while handling error");
                HTTPResponseInfo_destroy(response);
                cJSON_Delete(jsonData);
                return 0;
            }
            if(strcmp(errcode->valuestring,"M_ROOM_IN_USE")==0)
                showErrorDialog("This room already exists");
            else if(strcmp(errcode->valuestring,"M_INVALID_ROOM_STATE")==0)
                showErrorDialog("Invalid room state");
            else if(strcmp(errcode->valuestring,"M_UNSUPPORTED_ROOM_VERSION")==0)
                showErrorDialog("Unsupported room version");
            else
                showErrorDialog("Unknown error");
            HTTPResponseInfo_destroy(response);
            cJSON_Delete(jsonData);
            return 0;
        }
        showErrorDialog("Unexpected HTTP code while handling error");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog("Failed to parse response");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* roomID=cJSON_GetObjectItemCaseSensitive(jsonData,"room_id");
    if(!roomID){
        showErrorDialog("Failed to parse response");
        HTTPResponseInfo_destroy(response);
        cJSON_Delete(jsonData);
        return 0;
    }
    if(!matrix_joinRoomWithID(roomID->valuestring)){
        cJSON_Delete(jsonData);
        return 0;
    }
    char* id=malloc(strlen(roomID->valuestring)+1);
    strcpy(id,roomID->valuestring);
    cJSON_Delete(jsonData);
    return id;
}
char* matrix_getRoomAliasByID(char* id){
    int pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/aliases",id);
    char* path=malloc(pathLength+1);
    snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/aliases",id);
    char* url=http_toHTTPURL(path);
    free(path);
    http_sendGETRequest(url,app->loginInfo->homeserverName,app->homeserverSocket,app->loginInfo->accessToken);
    HTTPResponseInfo* response=http_readResponse(app->homeserverSocket);
    if(strcmp(response->datatype,"application/json")!=0){
        showErrorDialog("Server responded with unexpected datatype");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog("Failed to parse response");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* aliases=cJSON_GetObjectItemCaseSensitive(jsonData,"aliases");
    if(!cJSON_IsArray(aliases)){
        showErrorDialog("Failed to parse response: aliases is not array");
        cJSON_Delete(jsonData);
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* aliasesItem=cJSON_GetArrayItem(aliases,0);
    if(!cJSON_IsString(aliasesItem)){
        showErrorDialog("Failed to parse response: aliases item is not string");
        cJSON_Delete(jsonData);
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    char* alias=malloc(strlen(aliasesItem->valuestring)+1);
    strcpy(alias,aliasesItem->valuestring);
    cJSON_Delete(jsonData);
    HTTPResponseInfo_destroy(response);
    return alias;
}
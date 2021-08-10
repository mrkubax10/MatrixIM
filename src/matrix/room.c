#include "matrix/room.h"
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "http/http.h"
#include "app.h"
#include "screens/main_screen.h"
MatrixRoom* MatrixRoom_new(){
    MatrixRoom* output=(MatrixRoom*)malloc(sizeof(MatrixRoom));
    return output;
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
        printf("%s\n",response->data);
        if(jsonData){
            cJSON* error=cJSON_GetObjectItemCaseSensitive(jsonData,"error");
            if(error){
                int length=snprintf(0,0,"Failed to enter room: %s",error->valuestring);
                free(errorMsg);
                errorMsg=(char*)malloc(length+1);
                snprintf(errorMsg,length+1,"Failed to enter room: %s",error->valuestring);
                cJSON_free((void*)error);
            }
            cJSON_free((void*)jsonData);
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
        cJSON_free((void*)jsonData);
        return 0;
    }
    HTTPResponseInfo_destroy(response);
    for(unsigned int i=0; i<mainScreen->enteredRooms->capacity; i++){
        MatrixRoom* matrixRoom=mainScreen->enteredRooms->data[i];
        if(strcmp(matrixRoom->roomID,roomID->valuestring)==0){
            showInfoDialog("You already entered this room");
            return 0;
        }
    }
    pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/join",roomID->valuestring);
    path=(char*)malloc(pathLength+1);
    snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/join",roomID->valuestring);
    http_sendPOSTRequest(path,app->loginInfo->homeserverName,"application/json",0,"",app->loginInfo->accessToken,app->homeserverSocket);

    char* output=malloc(strlen(roomID->valuestring)+1);
    strcpy(output,roomID->valuestring);
    cJSON_free((void*)roomID);
    cJSON_free((void*)jsonData);
    free(path);
    Socket_read(app->homeserverSocket,responseData,4096);
    response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        showErrorDialog("Failed to enter room");
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    HTTPResponseInfo_destroy(response);
    return output;
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
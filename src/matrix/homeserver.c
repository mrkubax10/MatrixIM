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

#include "matrix/homeserver.h"
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "app.h"
#include "http/http.h"
#include "utils/message.h"
#include "utils/log.h"
#include "translation/translation.h"
HomeserverInfo* HomeserverInfo_new(){
    HomeserverInfo* output=malloc(sizeof(HomeserverInfo));
    output->supportsLoginPassword=false;
    output->supportsLoginRecaptcha=false;
    output->supportsLoginOAuth2=false;
    output->supportsLoginSSO=false;
    output->supportsLoginEmailIdentity=false;
    output->supportsLoginMsisdn=false;
    output->supportsLoginToken=false;
    output->supportsLoginDummy=false;
    return output;
}
void HomeserverInfo_destroy(HomeserverInfo* homeserver){
    free(homeserver);
}
HomeserverInfo* matrix_getHomeserverInfo(char* ip,int port){
    char responseData[4096];
    http_sendGETRequest("/_matrix/client/r0/login",ip,app->homeserverSocket,0);
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        log_warning("Homeserver","Server returned HTTP code: %d while getting available login methods",response->code);
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog(_("Failed to parse response"));
        HTTPResponseInfo_destroy(response);
        return 0;
    }
    HTTPResponseInfo_destroy(response);
    cJSON* flows=cJSON_GetObjectItemCaseSensitive(jsonData,"flows");
    if(!cJSON_IsArray(flows)){
        showErrorDialog(_("Failed to parse response: flows is not array"));
        return 0;
    }
    HomeserverInfo* homeserver=HomeserverInfo_new();
    cJSON* flowsItem=cJSON_GetArrayItem(flows,0);
    while(cJSON_IsObject(flowsItem)){
        cJSON* flowsItemType=cJSON_GetObjectItemCaseSensitive(flowsItem,"type");
        if(!flowsItemType || !cJSON_IsString(flowsItemType)){
            showErrorDialog(_("Failed to parse response: type attribute of Flow object is not string"));
            HomeserverInfo_destroy(homeserver);
            return 0;
        }
        if(strcmp(flowsItemType->valuestring,"m.login.password")==0){
            homeserver->supportsLoginPassword=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.recaptcha")==0){
            homeserver->supportsLoginRecaptcha=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.oauth2")==0){
            homeserver->supportsLoginOAuth2=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.sso")==0){
            homeserver->supportsLoginSSO=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.email.identity")==0){
            homeserver->supportsLoginEmailIdentity=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.msisdn")==0){
            homeserver->supportsLoginMsisdn=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.token")==0){
            homeserver->supportsLoginToken=true;
        }
        else if(strcmp(flowsItemType->valuestring,"m.login.dummy")==0){
            homeserver->supportsLoginDummy=true;
        }
        cJSON_free((void*)flowsItemType);
        flowsItem=flowsItem->next;
    }
    cJSON_free((void*)jsonData);
    return homeserver;
}

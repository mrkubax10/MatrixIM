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

#include "app.h"

#include "screens/login_screen.h"
#include <cjson/cJSON.h>
#include "utils/filesystem.h"
#include <stdlib.h>

#include "utils/log.h"
Application* app;
ApplicationSettings* ApplicationSettings_new(){
    ApplicationSettings* output=malloc(sizeof(ApplicationSettings));
    output->wasLoggedIn=false;
    output->lastPort=8008;
    output->deviceID=0;
    output->lastPassword=0;
    output->lastUsername=0;
    return output;
}
void ApplicationSettings_load(ApplicationSettings* settings){
    char* configDirCopy=malloc(strlen(app->configDir)+strlen("/config.json")+1);
    strcpy(configDirCopy,app->configDir);
    strcat(configDirCopy,"/config.json");
    char* configData=loadFullFile(configDirCopy);
    log_info("Config","Loading config from %s",configDirCopy);
    if(!configData){
        log_warning("Config","Failed to load config, using defaults instead");
        free(configDirCopy);
        return;
    }
    free(configDirCopy);
    const cJSON* jsonData=cJSON_Parse(configData);
    if(!jsonData){
        log_warning("Config","Failed to parse config");
        free(configData);
        return;
    }
    const cJSON* lastPort=cJSON_GetObjectItemCaseSensitive(jsonData,"lastPort");
    const cJSON* lastUsername=cJSON_GetObjectItemCaseSensitive(jsonData,"lastUsername");
    const cJSON* lastPassword=cJSON_GetObjectItemCaseSensitive(jsonData,"lastPassword");
    const cJSON* wasLoggedIn=cJSON_GetObjectItemCaseSensitive(jsonData,"wasLoggedIn");
    const cJSON* deviceID=cJSON_GetObjectItemCaseSensitive(jsonData,"deviceID");
    const cJSON* lastHomeserver=cJSON_GetObjectItemCaseSensitive(jsonData,"lastHomeserver");
    if(lastPort && cJSON_IsNumber(lastPort)){
        settings->lastPort=lastPort->valueint;
        cJSON_free((void*)lastPort);
    }
    if(lastUsername && cJSON_IsString(lastUsername)){
        settings->lastUsername=(char*)malloc(strlen(lastUsername->valuestring)+1);
        strcpy(settings->lastUsername,lastUsername->valuestring);
        cJSON_free((void*)lastUsername);
    }
    if(lastPassword && cJSON_IsString(lastPassword)){
        settings->lastPassword=(char*)malloc(strlen(lastPassword->valuestring)+1);
        strcpy(settings->lastPassword,lastPassword->valuestring);
        cJSON_free((void*)lastPassword);
    }
    if(wasLoggedIn && cJSON_IsBool(wasLoggedIn)){
        settings->wasLoggedIn=wasLoggedIn->valueint==1?true:false;
        cJSON_free((void*)wasLoggedIn);
    }
    if(deviceID && cJSON_IsString(deviceID)){
        settings->deviceID=(char*)malloc(strlen(deviceID->valuestring)+1);
        strcpy(settings->deviceID,deviceID->valuestring);
        cJSON_free((void*)deviceID);
    }
    if(lastHomeserver && cJSON_IsString(lastHomeserver)){
        settings->lastHomeserver=(char*)malloc(strlen(lastHomeserver->valuestring)+1);
        strcpy(settings->lastHomeserver,lastHomeserver->valuestring);
        cJSON_free((void*)lastHomeserver);
    }
    cJSON_free((void*)jsonData);
    free(configData);
}
void ApplicationSettings_save(ApplicationSettings* settings){
    char* configDirCopy=malloc(strlen(app->configDir)+strlen("/config.json")+1);
    strcpy(configDirCopy,app->configDir);
    strcat(configDirCopy,"/config.json");
    log_info("Config","Saving config to %s",configDirCopy);
    if(!folderExists(app->configDir))
        createFolder(app->configDir);
    FILE* file=fopen(configDirCopy,"w");
    if(!file){
        log_warning("Config","Failed to save config");
        free(configDirCopy);
        return;
    }
    free(configDirCopy);
    cJSON* root=cJSON_CreateObject();
    if(!root){
        log_warning("Config","Failed to save config");
        fclose(file);
        return;
    }
    cJSON_AddNumberToObject(root,"lastPort",app->settings->lastPort);
    cJSON_AddStringToObject(root,"lastUsername",app->settings->lastUsername);
    cJSON_AddStringToObject(root,"lastPassword",app->settings->lastPassword);
    cJSON_AddStringToObject(root,"lastHomeserver",app->settings->lastHomeserver);
    cJSON_AddBoolToObject(root,"wasLoggedIn",app->settings->wasLoggedIn);
    cJSON_AddStringToObject(root,"deviceID",app->settings->deviceID);
    char* stringData=cJSON_Print(root);
    if(!stringData){
        log_warning("Config","Failed to save config");
        cJSON_free(root);
        fclose(file);
        return;
    }
    fwrite(stringData,1,strlen(stringData),file);
    cJSON_free(root);
    fclose(file);
}
Application* Application_new(){
    Application* output=malloc(sizeof(Application));
    output->loginInfo=LoginInfo_new();
    output->loggedIn=false;
    output->configDir=getConfigDir();
    output->settings=ApplicationSettings_new();
    return output;
}
void application_activate(GtkApplication* gtkApplication,gpointer userdata){
    app->window=gtk_application_window_new(gtkApplication);
    app->homeserverSocket=Socket_new();
    app->fixedContainer=gtk_fixed_new();
    gtk_window_set_title(GTK_WINDOW(app->window),"MatrixIM");
    gtk_window_set_default_size(GTK_WINDOW(app->window),400,350);
    loginscreen_init();
}

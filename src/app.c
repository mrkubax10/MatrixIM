#include "app.h"
#include "screens/login_screen.h"
#include <cjson/cJSON.h>
#include "utils/filesystem.h"
#include <stdlib.h>
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
    printf("(Log) [Config] Loading config from %s\n",configDirCopy);
    if(!configData){
        printf("(Warn) [Config] Failed to load config, using defaults instead\n");
        free(configDirCopy);
        return;
    }
    free(configDirCopy);
    const cJSON* jsonData=cJSON_Parse(configData);
    if(!jsonData){
        printf("(Warn) [Config] Failed to parse settings\n");
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
    printf("(Log) [Config] Saving config to %s\n",configDirCopy);
    if(!folderExists(app->configDir))
        createFolder(app->configDir);
    FILE* file=fopen(configDirCopy,"w");
    if(!file){
        printf("(Warn) [Config] Failed to save config\n");
        free(configDirCopy);
        return;
    }
    free(configDirCopy);
    cJSON* root=cJSON_CreateObject();
    if(!root){
        printf("(Warn) [Config] Failed to save config\n");
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
        printf("(Warn) [Config] Failed to save config\n");
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

#include "app.h"
#include "screens/login_screen.h"
#include <cjson/cJSON.h>
Application* app;
ApplicationSettings* ApplicationSettings_new(){
    ApplicationSettings* output=(ApplicationSettings*)malloc(sizeof(ApplicationSettings));
    output->wasLoggedIn=false;
    output->lastPort=8008;
    output->deviceID=0;
    output->lastPassword=0;
    output->lastUsername=0;
    return output;
}
void ApplicationSettings_load(ApplicationSettings* settings){
    char* configDirCopy=(char*)malloc(strlen(app->configDir)+1);
    strcpy(configDirCopy,app->configDir);
    FILE* file=fopen(strcat(configDirCopy,"/config.json"),"r");
    if(!file){
        printf("(Warn) [Config] Failed to load config, using defaults instead\n");
        free(configDirCopy);
        return;
    }
    free(configDirCopy);
    fseek(file,0,SEEK_END);
    int fileSize=ftell(file);
    fseek(file,0,SEEK_SET);
    char* data=(char*)malloc(fileSize+1);
    int readDataCount=fread(data,1,fileSize,file);
    if(readDataCount!=fileSize)
        printf("(Warn) [Config] Failed to load full file, errorcode: %d\n",ferror(file));
    const cJSON* jsonData=cJSON_Parse(data);
    if(!jsonData){
        printf("(Warn) [Config] Failed to parse settings\n");
        fclose(file);
        free(data);
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
        cJSON_free(lastPort);
    }
    if(lastUsername && cJSON_IsString(lastUsername)){
        settings->lastUsername=(char*)malloc(strlen(lastUsername->valuestring)+1);
        strcpy(settings->lastUsername,lastUsername->valuestring);
        cJSON_free(lastUsername);
    }
    if(lastPassword && cJSON_IsString(lastPassword)){
        settings->lastPassword=(char*)malloc(strlen(lastPassword->valuestring)+1);
        strcpy(settings->lastPassword,lastPassword->valuestring);
        cJSON_free(lastPassword);
    }
    if(wasLoggedIn && cJSON_IsBool(wasLoggedIn)){
        settings->wasLoggedIn=wasLoggedIn->valueint==1?true:false;
        cJSON_free(wasLoggedIn);
    }
    if(deviceID && cJSON_IsString(deviceID)){
        settings->deviceID=(char*)malloc(strlen(deviceID->valuestring)+1);
        strcpy(settings->deviceID,deviceID->valuestring);
        cJSON_free(deviceID);
    }
    if(lastHomeserver && cJSON_IsString(lastHomeserver)){
        settings->lastHomeserver=(char*)malloc(strlen(lastHomeserver->valuestring)+1);
        strcpy(settings->lastHomeserver,lastHomeserver->valuestring);
        cJSON_free(lastHomeserver);
    }
    cJSON_free(jsonData);
    fclose(file);
}
void ApplicationSettings_save(ApplicationSettings* settings){
    char* configDirCopy=(char*)malloc(strlen(app->configDir)+1);
    strcpy(configDirCopy,app->configDir);
    printf("(Log) [Config] Saving config to %s\n",strcat(configDirCopy,"/config.json"));
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
void application_activate(GtkApplication* gtkApplication,gpointer userdata){
    app->window=gtk_application_window_new(gtkApplication);
    app->homeserverSocket=Socket_new();
    gtk_window_set_title(GTK_WINDOW(app->window),"MatrixIM");
    gtk_window_set_default_size(GTK_WINDOW(app->window),400,350);
    loginscreen_init();
    gtk_widget_show_all(app->window);
}
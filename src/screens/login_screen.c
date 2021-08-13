#include "screens/login_screen.h"
#include "app.h"
#include <stdio.h>
#include "http/http.h"
#include <cjson/cJSON.h>
#include "matrix/login.h"
#include "matrix/register.h"
#include "utils.h"
#include <stdlib.h>
#include "screens/main_screen.h"
#include "utils/message.h"
LoginScreen* loginScreen;
LoginScreen* LoginScreen_new(){
    LoginScreen* output=(LoginScreen*)malloc(sizeof(LoginScreen));
    return output;
}

void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData){
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Username cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    if(strcmp(password,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Password cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    char* ip=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryHomeserver));
    char* port=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPort));
    if(strcmp(ip,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Homeserver IP cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    if(strcmp(port,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Homeserver port cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    LoginResult loginResult=loginscreen_login(ip,atoi(port),username,password);
    loginscreen_checkLoginResult(loginResult);
    if(loginResult==LOGINRESULT_SUCCESS){
        loginscreen_finish();
        mainscreen_init();
    }
}
void loginscreen_buttonRegister_clicked(GtkWidget* widget,gpointer userData){
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Username cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    if(strcmp(password,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Password cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    char* ip=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryHomeserver));
    char* port=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPort));
    if(strcmp(ip,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Homeserver IP cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    if(strcmp(port,"")==0){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Homeserver port cannot be empty");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        return;
    }
    RegisterResult registerResult=loginscreen_register(ip,atoi(port),username,password);
    loginscreen_checkRegisterResult(registerResult);
    if(registerResult==REGISTERRESULT_SUCCESS){
        loginscreen_finish();
        mainscreen_init();
    }
}
void loginscreen_showPassword_toggle(GtkWidget* widget,gpointer userData){
    gtk_entry_set_visibility(GTK_ENTRY(loginScreen->entryPassword),!gtk_entry_get_visibility(GTK_ENTRY(loginScreen->entryPassword)));
}
void loginscreen_init(){
    loginScreen=LoginScreen_new();
    GtkBuilder* builder=gtk_builder_new();
    if(!gtk_builder_add_from_file(GTK_BUILDER(builder),"ui/matrixim_login_screen.ui",0)){
        showErrorDialog("Failed to load UI file matrixim_login_screen.ui");
        exit(0);
    }
    gtk_builder_connect_signals(builder,0);
    loginScreen->containerContent=GTK_WIDGET(gtk_builder_get_object(builder,"containerContent"));
    loginScreen->entryHomeserver=GTK_WIDGET(gtk_builder_get_object(builder,"entryHomeserverIP"));
    loginScreen->entryPort=GTK_WIDGET(gtk_builder_get_object(builder,"entryHomeserverPort"));
    loginScreen->entryUsername=GTK_WIDGET(gtk_builder_get_object(builder,"entryLogin"));
    loginScreen->entryPassword=GTK_WIDGET(gtk_builder_get_object(builder,"entryPassword"));
    if(app->settings->wasLoggedIn && app->settings->lastUsername && app->settings->lastPassword && app->settings->lastHomeserver){
        LoginResult loginResult=loginscreen_login(app->settings->lastHomeserver,app->settings->lastPort,app->settings->lastUsername,app->settings->lastPassword);
        loginscreen_checkLoginResult(loginResult);
        if(loginResult==LOGINRESULT_SUCCESS){
            mainscreen_init();
            return;
        }
    }
    else{
        if(app->settings->lastHomeserver!=0)
            gtk_entry_set_text(GTK_ENTRY(loginScreen->entryHomeserver),app->settings->lastHomeserver);
        if(app->settings->lastPort!=8008)
            gtk_entry_set_text(GTK_ENTRY(loginScreen->entryPort),intToString(app->settings->lastPort));
        if(app->settings->lastPassword){
            gtk_entry_set_text(GTK_ENTRY(loginScreen->entryPassword),app->settings->lastPassword);
        }
        if(app->settings->lastUsername)
            gtk_entry_set_text(GTK_ENTRY(loginScreen->entryUsername),app->settings->lastUsername);
    }
    
    gtk_entry_set_visibility(GTK_ENTRY(loginScreen->entryPassword),false);
    gtk_entry_set_text(GTK_ENTRY(loginScreen->entryPort),"8008");

    gtk_container_add(GTK_CONTAINER(app->window),loginScreen->containerContent);
    g_object_unref(builder);
    
    gtk_widget_show_all(app->window);
}
void loginscreen_finish(){
    gtk_container_foreach(GTK_CONTAINER(app->window),(GtkCallback)gtk_widget_destroy,0);
    free(loginScreen);
}
void loginscreen_checkLoginResult(LoginResult loginResult){
    if(loginResult!=LOGINRESULT_SUCCESS){
        char* information;
        switch(loginResult){
        case LOGINRESULT_INTERNAL_ERROR:
            information="Failed to connect to homeserver: internal error";
            break;
        case LOGINRESULT_SERVER_ERROR:
            information="Failed to connect to homeserver: server error";
            break;
        case LOGINRESULT_NETWORK_ERROR:
            information="Failed to connect to homeserver: network error";
            break;
        case LOGINRESULT_WRONG_USERNAME:
            information="Failed to login: wrong username";
            break;
        case LOGINRESULT_WRONG_PASSWORD:
            information="Failed to login: wrong password";
            break;
        case LOGINRESULT_UNSUPPORTED_LOGIN_TYPE:
            information="Failed to login: unsupported login type";
            break;
        case LOGINRESULT_UNSUPPORTED_MATRIX_REVISION:
            information="Failed to login: unsupported Matrix revision";
            break;
        case LOGINRESULT_WRONG_USERNAME_OR_PASSWORD:
            information="Failed to login: wrong username or password";
            break;
        default:
            information="Unknown error";
            break;
        }
        Socket_close(app->homeserverSocket);
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s",information);
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
    }
}
LoginResult loginscreen_login(char* ip,int port,char* username,char* password){
    if(!app->loggedIn){
        if(!Socket_connect(app->homeserverSocket,ip,port))
            return REGISTERRESULT_NETWORK_ERROR;
    }
    char responseData[4096];
    http_sendGETRequest("/_matrix/client/r0/login",ip,app->homeserverSocket,0);
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        printf("(Warn) [Login] Server returned HTTP code: %d while getting available login methods\n",response->code);
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_SERVER_ERROR;
    }
    if(strcmp(response->datatype,"application/json")!=0){
        printf("(Warn) [Login] Server responded with unexpected data type (%s)\n",response->datatype);
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_INTERNAL_ERROR;
    }
    const cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_INTERNAL_ERROR;
    }
    const cJSON* flows=cJSON_GetObjectItemCaseSensitive(jsonData,"flows");
    if(!flows){
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_INTERNAL_ERROR;
    }
    if(!cJSON_IsArray(flows)){
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_INTERNAL_ERROR;
    }
    // Check for supported auth methods
    // At the moment we only support m.login.password
    bool foundLoginWithPassword=false;
    int flowsIndex=0;
    const cJSON* flowsItem;
    while((flowsItem=cJSON_GetArrayItem(flows,flowsIndex))){
        const cJSON* itemType=cJSON_GetObjectItemCaseSensitive(flowsItem,"type");
        if(itemType){
            if(strcmp(itemType->valuestring,"m.login.password")==0){
                foundLoginWithPassword=true;
                cJSON_free((void*)itemType);
                break;
            }
        }
        cJSON_free((void*)itemType);
        flowsIndex++;
    }
    cJSON_free((void*)flowsItem);
    cJSON_free((void*)flows);
    cJSON_free((void*)jsonData);
    if(!foundLoginWithPassword){
        HTTPResponseInfo_destroy(response);
        return LOGINRESULT_UNSUPPORTED_LOGIN_TYPE;
    }
    
    HTTPResponseInfo_destroy(response);
    char* loginRequest;
    if(app->settings->deviceID!=0)
        loginRequest=matrix_createPasswordLoginRequest(username,password,0,app->settings->deviceID);
    else
        loginRequest=matrix_createPasswordLoginRequest(username,password,"MatrixIM",0);
    http_sendPOSTRequest("/_matrix/client/r0/login",ip,"application/json",strlen(loginRequest),loginRequest,0,app->homeserverSocket);
    free(loginRequest);
    Socket_read(app->homeserverSocket,responseData,4096);
    response=http_parseResponse(responseData);
    jsonData=cJSON_Parse(response->data);
    if(response->code!=HTTP_CODE_OK){
        if(strcmp(response->datatype,"application/json")==0){
            const cJSON* jsonErrcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
            if(jsonErrcode){
                if(strcmp(jsonErrcode->valuestring,"M_UNRECOGNIZED")==0){
                    HTTPResponseInfo_destroy(response);
                    cJSON_free((void*)jsonErrcode);
                    cJSON_free((void*)jsonData);
                    return LOGINRESULT_UNSUPPORTED_LOGIN_TYPE;
                }
                else if(strcmp(jsonErrcode->valuestring,"M_FORBIDDEN")==0){
                    HTTPResponseInfo_destroy(response);
                    cJSON_free((void*)jsonErrcode);
                    cJSON_free((void*)jsonData);
                    return LOGINRESULT_WRONG_USERNAME_OR_PASSWORD;
                }
            }
            else{
                HTTPResponseInfo_destroy(response);
                cJSON_free((void*)jsonErrcode);
                cJSON_free((void*)jsonData);
                return LOGINRESULT_INTERNAL_ERROR;
            }
        }
        else{
            HTTPResponseInfo_destroy(response);
            cJSON_free((void*)jsonData);
            return LOGINRESULT_INTERNAL_ERROR;
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
        return LOGINRESULT_INTERNAL_ERROR;
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
        return LOGINRESULT_INTERNAL_ERROR;
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
    app->settings->lastPort=port;
    app->settings->lastUsername=(char*)malloc(strlen(username)+1);
    strcpy(app->settings->lastUsername,username);
    app->settings->lastPassword=(char*)malloc(strlen(password)+1);
    strcpy(app->settings->lastPassword,password);
    app->settings->lastHomeserver=(char*)malloc(strlen(ip)+1);
    strcpy(app->settings->lastHomeserver,ip);
    return LOGINRESULT_SUCCESS;
}
RegisterResult loginscreen_register(char* ip,int port,char* username,char* password){
    if(!app->loggedIn){
        if(!Socket_connect(app->homeserverSocket,ip,port))
            return REGISTERRESULT_NETWORK_ERROR;
    }
    char responseData[4096];
    char* availableUsernameRequest=(char*)malloc(strlen("/_matrix/client/r0/register/available?username=")+1);
    strcpy(availableUsernameRequest,"/_matrix/client/r0/register/available?username=");
    strcat(availableUsernameRequest,username);
    http_sendGETRequest(availableUsernameRequest,ip,app->homeserverSocket,0);
    free(availableUsernameRequest);
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* responseInfo=http_parseResponse(responseData);
    if(strcmp(responseInfo->datatype,"application/json")!=0){
        printf("(Warn) [Register] Server responded with unexpected data type (%s)\n",responseInfo->datatype);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_WRONG_DATATYPE;
    }
    cJSON* jsonData=cJSON_Parse(responseInfo->data);
    cJSON* available=cJSON_GetObjectItemCaseSensitive(jsonData,"available");
    if(!available){
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_USERNAME_INVALID;
    }
    if(!cJSON_IsBool(available)){
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_INTERNAL_ERROR;
    }
    if(!available->valueint){
        cJSON_free((void*)jsonData);
        cJSON_free((void*)available);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_USERNAME_INVALID;
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
        printf("(Warn) [Register] Server responded with unexpected data type (%s)\n",responseInfo->datatype);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_WRONG_DATATYPE;
    }
    jsonData=cJSON_Parse(responseInfo->data);
    if(!jsonData){
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_INTERNAL_ERROR;
    }
    if(responseInfo->code!=HTTP_CODE_UNAUTHORIZED){
        cJSON* errcode=cJSON_GetObjectItemCaseSensitive(jsonData,"errcode");
        if(!errcode){
            cJSON_free((void*)jsonData);
            HTTPResponseInfo_destroy(responseInfo);
            return REGISTERRESULT_INTERNAL_ERROR;
        }
        if(strcmp(errcode->valuestring,"M_WEAK_PASSWORD")==0){ // Password was too weak
            cJSON_free((void*)jsonData);
            cJSON_free((void*)errcode);
            HTTPResponseInfo_destroy(responseInfo);
            return REGISTERRESULT_WEAK_PASSWORD;
        }
        else if(strcmp(errcode->valuestring,"M_UNKNOWN")==0){ // Server didn't recognize provided auth method
            cJSON_free((void*)jsonData);
            cJSON_free((void*)errcode);
            HTTPResponseInfo_destroy(responseInfo);
            return REGISTERRESULT_SERVER_ERROR;
        }
        cJSON_free((void*)jsonData);
        cJSON_free((void*)errcode);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_UNKNOWN_ERROR;
    }
    cJSON* session=cJSON_GetObjectItemCaseSensitive(jsonData,"session");
    if(!session){
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_INTERNAL_ERROR;
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
        return REGISTERRESULT_INTERNAL_ERROR;
    
    }
    jsonData=cJSON_Parse(responseData);
    if(!jsonData){
        HTTPResponseInfo_destroy(responseInfo);
        return REGISTERRESULT_INTERNAL_ERROR;
    }
    cJSON* jsonAccessToken=cJSON_GetObjectItemCaseSensitive(jsonData,"access_token");
    cJSON* jsonUserID=cJSON_GetObjectItemCaseSensitive(jsonData,"user_id");
    cJSON* jsonDeviceID=cJSON_GetObjectItemCaseSensitive(jsonData,"device_id");
    if(!jsonUserID || !jsonAccessToken || !jsonDeviceID){
        cJSON_free((void*)jsonUserID);
        cJSON_free((void*)jsonAccessToken);
        cJSON_free((void*)jsonDeviceID);
        cJSON_free((void*)jsonData);
        HTTPResponseInfo_destroy(responseInfo);
        return LOGINRESULT_INTERNAL_ERROR;
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
        return LOGINRESULT_INTERNAL_ERROR;
    }
    app->loginInfo->homeserverName=(char*)malloc(strlen(userIDData[1])+1);
    strcpy(app->loginInfo->homeserverName,userIDData[1]);
    printf("(Log) [Login] User ID: %s\n",jsonUserID->valuestring);
    array_free((void**)userIDData,userIDDataLength);
    HTTPResponseInfo_destroy(responseInfo);
    cJSON_free((void*)jsonUserID);
    cJSON_free((void*)jsonAccessToken);
    cJSON_free((void*)jsonDeviceID);
    cJSON_free((void*)jsonData);
    app->loggedIn=true;
    app->settings->wasLoggedIn=true;
    app->settings->deviceID=app->loginInfo->deviceID;
    app->settings->lastPort=port;
    app->settings->lastUsername=(char*)malloc(strlen(username)+1);
    strcpy(app->settings->lastUsername,username);
    app->settings->lastPassword=(char*)malloc(strlen(password)+1);
    strcpy(app->settings->lastPassword,password);
    app->settings->lastHomeserver=(char*)malloc(strlen(ip)+1);
    strcpy(app->settings->lastHomeserver,ip);
    return REGISTERRESULT_SUCCESS;
}
void loginscreen_checkRegisterResult(RegisterResult registerResult){
    if(registerResult!=REGISTERRESULT_SUCCESS){
        char* information;
        switch(registerResult){
        case REGISTERRESULT_INTERNAL_ERROR:
            information="Failed to register: internal error";
            break;
        case REGISTERRESULT_SERVER_ERROR:
            information="Failed to register: server error";
            break;
        case REGISTERRESULT_NETWORK_ERROR:
            information="Failed to connect to homeserver: network error";
            break;
        case REGISTERRESULT_USERNAME_INVALID:
            information="Failed to register: username already taken or it is invalid";
            break;
        case REGISTERRESULT_WEAK_PASSWORD:
            information="Failed to register: password too weak";
            break;
        case REGISTERRESULT_UNSUPPORTED_AUTH:
            information="Failed to register: unsupported auth";
            break;
        case REGISTERRESULT_WRONG_DATATYPE:
            information="Failed to register: server responded with unexpected datatype";
            break;
        default:
            information="Unknown error";
            break;
        }
        Socket_close(app->homeserverSocket);
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s",information);
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
    }
}
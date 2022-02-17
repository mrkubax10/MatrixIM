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

#include "screens/login_screen.h"

#include <stdio.h>
#include <cjson/cJSON.h>
#include <stdlib.h>

#include "app.h"
#include "http/http.h"
#include "matrix/login.h"
#include "matrix/register.h"
#include "matrix/homeserver.h"
#include "screens/main_screen.h"
#include "utils/message.h"
#include "utils/str.h"
#include "translation/translation.h"

typedef struct SelectHomeserverData{
    char* ip;
    int port;
    bool autologinPassword;
} SelectHomeserverData;

LoginScreen* loginScreen;
LoginScreen* LoginScreen_new(){
    LoginScreen* output=(LoginScreen*)malloc(sizeof(LoginScreen));
    output->buttonSelectHomeserver=0;
    return output;
}

void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData){
    if(!loginScreen->lastSelectedHomeserver){
        showInfoDialog(_("Select homeserver first"));
        return;
    }
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        showInfoDialog(_("Username field cannot be empty"));
        return;
    }
    if(strcmp(password,"")==0){
        showInfoDialog(_("Password field cannot be empty"));
        return;
    }
    char* ip=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryHomeserver));
    if(matrix_loginPassword(ip,username,password,"MatrixIM",app->settings->deviceID)){
        loginscreen_finish();
        mainscreen_init();
    }
}
void loginscreen_buttonRegister_clicked(GtkWidget* widget,gpointer userData){
    if(!loginScreen->lastSelectedHomeserver){
        showInfoDialog(_("Select homeserver first"));
        return;
    }
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        showInfoDialog(_("Username field cannot be empty"));
        return;
    }
    if(strcmp(password,"")==0){
        showInfoDialog(_("Password field cannot be empty"));
        return;
    }
    if(matrix_registerPassword(loginScreen->lastSelectedHomeserver,username,password)){
        loginscreen_finish();
        mainscreen_init();
    }
}
void loginscreen_buttonHomeserverSelect_clicked(GtkWidget* widget,gpointer userData){
    char* ip=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryHomeserver));
    const char* port=gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPort));
    if(strcmp(ip,"")==0){
        showInfoDialog(_("Homeserver IP cannot be empty"));
        return;
    }
    if(strcmp(port,"")==0){
        showInfoDialog(_("Homeserver port cannot be empty"));
        return;
    }
    SelectHomeserverData* homeserverData=malloc(sizeof(SelectHomeserverData));
    homeserverData->ip=ip;
    homeserverData->port=atoi(port);
    homeserverData->autologinPassword=false;
    g_thread_new("homeserver",loginscreen_selectHomeserver,(gpointer)homeserverData);
}
void loginscreen_showPassword_toggle(GtkWidget* widget,gpointer userData){
    gtk_entry_set_visibility(GTK_ENTRY(loginScreen->entryPassword),!gtk_entry_get_visibility(GTK_ENTRY(loginScreen->entryPassword)));
}
void loginscreen_init(){
    loginScreen=LoginScreen_new();
    loginScreen->lastSelectedHomeserver=0;
    GtkBuilder* builder=gtk_builder_new();
    if(!gtk_builder_add_from_file(GTK_BUILDER(builder),"ui/matrixim_login_screen.ui",0)){
        char msg[512];
        snprintf(msg,512,_("Failed to load UI file %s"),"matrixim_login_screen.ui");
        showErrorDialog(msg);
        exit(0);
    }
    gtk_builder_connect_signals(builder,0);
    loginScreen->containerContent=GTK_WIDGET(gtk_builder_get_object(builder,"containerContent"));
    loginScreen->entryHomeserver=GTK_WIDGET(gtk_builder_get_object(builder,"entryHomeserverIP"));
    loginScreen->entryPort=GTK_WIDGET(gtk_builder_get_object(builder,"entryHomeserverPort"));
    loginScreen->entryUsername=GTK_WIDGET(gtk_builder_get_object(builder,"entryLogin"));
    loginScreen->entryPassword=GTK_WIDGET(gtk_builder_get_object(builder,"entryPassword"));
    loginScreen->buttonLogin=GTK_WIDGET(gtk_builder_get_object(builder,"buttonLogin"));
    loginScreen->buttonRegister=GTK_WIDGET(gtk_builder_get_object(builder,"buttonRegister"));
    if(app->settings->wasLoggedIn && app->settings->lastUsername && app->settings->lastPassword && app->settings->lastHomeserver){
        SelectHomeserverData* homeserverData=malloc(sizeof(SelectHomeserverData));
        homeserverData->ip=app->settings->lastHomeserver;
        homeserverData->port=app->settings->lastPort;
        homeserverData->autologinPassword=true;
        g_thread_new("homeserver",loginscreen_selectHomeserver,(gpointer)homeserverData);
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
static int loginscreen_selectHomeserver_updateGUI(gpointer arg){
    HomeserverInfo* homeserver=(HomeserverInfo*)((gpointer*)arg)[0];
    SelectHomeserverData* selectData=(SelectHomeserverData*)((gpointer*)arg)[1];
    if(!homeserver->supportsLoginPassword){
        gtk_widget_set_sensitive(loginScreen->entryUsername,false);
        gtk_widget_set_sensitive(loginScreen->entryPassword,false);
        gtk_widget_set_sensitive(loginScreen->buttonLogin,false);
        gtk_widget_set_sensitive(loginScreen->buttonRegister,false);
    }
    if(loginScreen->lastSelectedHomeserver)
        free(loginScreen->lastSelectedHomeserver);
    loginScreen->lastSelectedHomeserver=malloc(strlen(selectData->ip)+1);
    strcpy(loginScreen->lastSelectedHomeserver,selectData->ip);
    app->settings->lastHomeserver=loginScreen->lastSelectedHomeserver;
    app->settings->lastPort=selectData->port;
    HomeserverInfo_destroy(homeserver);
    if(selectData->autologinPassword){
        free(selectData);
        free(arg);
        if(matrix_loginPassword(app->settings->lastHomeserver,app->settings->lastUsername,app->settings->lastPassword,"MatrixIM",app->settings->deviceID)){
            loginscreen_finish();
            mainscreen_init();
        }
    }
    return 0;
}
static int loginscreen_selectHomeserver_showError(gpointer arg){
    showErrorDialog(_("Failed to connect to homeserver"));
    return 0;
}
void* loginscreen_selectHomeserver(void* arg){
    char* ip=((SelectHomeserverData*)arg)->ip;
    int port=((SelectHomeserverData*)arg)->port;
    if(Socket_isConnected(app->homeserverSocket)){
        if(strcmp(ip,app->settings->lastHomeserver)==0 && port==app->settings->lastPort){
            free(arg);
            return 0;
        }
        Socket_disconnect(app->homeserverSocket);
    }
    if(!Socket_connect(app->homeserverSocket,ip,port)){
        g_idle_add(loginscreen_selectHomeserver_showError,0);
        free(arg);
        return 0;
    }
    HomeserverInfo* homeserver=matrix_getHomeserverInfo(ip,port);
    if(!homeserver){
        free(arg);
        return 0;
    }
    gpointer* data=malloc(sizeof(gpointer)*2);
    data[0]=(gpointer)homeserver;
    data[1]=(gpointer)arg;
    g_idle_add(loginscreen_selectHomeserver_updateGUI,(gpointer)data);
    return 0;
}
#include "screens/login_screen.h"
#include "app.h"
#include <stdio.h>
#include "http/http.h"
#include <cjson/cJSON.h>
#include "matrix/login.h"
#include "matrix/register.h"
#include "matrix/homeserver.h"
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
    if(!loginScreen->lastSelectedHomeserver){
        showInfoDialog("Select homeserver first");
        return;
    }
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        showInfoDialog("Username field cannot be empty");
        return;
    }
    if(strcmp(password,"")==0){
        showInfoDialog("Password field cannot be empty");
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
        showInfoDialog("Select homeserver first");
        return;
    }
    char* username=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryUsername));
    char* password=(char*)gtk_entry_get_text(GTK_ENTRY(loginScreen->entryPassword));
    if(strcmp(username,"")==0){
        showInfoDialog("Username field cannot be empty");
        return;
    }
    if(strcmp(password,"")==0){
        showInfoDialog("Password field cannot be empty");
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
        showInfoDialog("Homeserver IP cannot be empty");
        return;
    }
    if(strcmp(port,"")==0){
        showInfoDialog("Homeserver port cannot be empty");
        return;
    }
    loginscreen_selectHomeserver(ip,atoi(port));
}
void loginscreen_showPassword_toggle(GtkWidget* widget,gpointer userData){
    gtk_entry_set_visibility(GTK_ENTRY(loginScreen->entryPassword),!gtk_entry_get_visibility(GTK_ENTRY(loginScreen->entryPassword)));
}
void loginscreen_init(){
    loginScreen=LoginScreen_new();
    loginScreen->lastSelectedHomeserver=0;
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
    loginScreen->buttonLogin=GTK_WIDGET(gtk_builder_get_object(builder,"buttonLogin"));
    loginScreen->buttonRegister=GTK_WIDGET(gtk_builder_get_object(builder,"buttonRegister"));
    if(app->settings->wasLoggedIn && app->settings->lastUsername && app->settings->lastPassword && app->settings->lastHomeserver){
        if(loginscreen_selectHomeserver(app->settings->lastHomeserver,app->settings->lastPort)){
            if(matrix_loginPassword(app->settings->lastHomeserver,app->settings->lastUsername,app->settings->lastPassword,"MatrixIM",app->settings->deviceID)){
                mainscreen_init();
                return;
            }
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
bool loginscreen_selectHomeserver(char* ip,int port){
    if(Socket_isConnected(app->homeserverSocket)){
        if(strcmp(ip,app->settings->lastHomeserver)==0 && port==app->settings->lastPort)
            return true;
        Socket_disconnect(app->homeserverSocket);
    }
    if(!Socket_connect(app->homeserverSocket,ip,port)){
        showErrorDialog("Failed to connect to homeserver");
        return false;
    }
    HomeserverInfo* homeserver=matrix_getHomeserverInfo(ip,port);
    if(!homeserver)
        return false;
    if(!homeserver->supportsLoginPassword){
        gtk_widget_set_sensitive(loginScreen->entryUsername,false);
        gtk_widget_set_sensitive(loginScreen->entryPassword,false);
        gtk_widget_set_sensitive(loginScreen->buttonLogin,false);
        gtk_widget_set_sensitive(loginScreen->buttonRegister,false);
    }
    if(loginScreen->lastSelectedHomeserver)
        free(loginScreen->lastSelectedHomeserver);
    loginScreen->lastSelectedHomeserver=malloc(strlen(ip)+1);
    strcpy(loginScreen->lastSelectedHomeserver,ip);
    app->settings->lastHomeserver=loginScreen->lastSelectedHomeserver;
    app->settings->lastPort=port;
    return true;
}
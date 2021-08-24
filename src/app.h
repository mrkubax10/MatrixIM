#ifndef SRC_APP_H
#define SRC_APP_H
#include <gtk/gtk.h>
#include "network/socket.h"
#include "matrix/login.h"
typedef struct ApplicationSettings{
    char* deviceID;
    char* lastUsername;
    char* lastPassword;
    char* lastHomeserver;
    int lastPort;
    bool wasLoggedIn;
} ApplicationSettings;
ApplicationSettings* ApplicationSettings_new();
void ApplicationSettings_load(ApplicationSettings* settings);
void ApplicationSettings_save(ApplicationSettings* settings);

typedef struct Application{
    GtkWidget* window;
    GtkWidget* fixedContainer;
    Socket* homeserverSocket;
    bool loggedIn;
    LoginInfo* loginInfo;
    char* configDir;
    ApplicationSettings* settings;
} Application;
Application* Application_new();
extern Application* app;
void application_activate(GtkApplication* gtkApplication,gpointer userData);
#endif

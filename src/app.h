#ifndef SRC_APP_H
#define SRC_APP_H
#include <gtk/gtk.h>
#include "network/socket.h"
typedef struct Application{
    GtkWidget* window;
    Socket* homeserverSocket;
} Application;
extern Application* app;
void application_activate(GtkApplication* gtkApplication,gpointer userData);
#endif
#ifndef SRC_APP_H
#define SRC_APP_H
#include <gtk/gtk.h>
typedef struct Application{
    GtkWidget* window;
} Application;
extern Application* app;
void application_activate(GtkApplication* gtkApplication,gpointer userData);
#endif
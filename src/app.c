#include "app.h"
#include "screens/login_screen.h"
Application* app;
void application_activate(GtkApplication* gtkApplication,gpointer userdata){
    app->window=gtk_application_window_new(gtkApplication);
    app->homeserverSocket=Socket_new();
    gtk_window_set_title(GTK_WINDOW(app->window),"MatrixIM");
    gtk_window_set_default_size(GTK_WINDOW(app->window),400,350);
    loginscreen_init();
    gtk_widget_show_all(app->window);
}
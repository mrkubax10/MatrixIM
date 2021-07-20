#ifndef SRC_SCREENS_LOGIN_SCREEN_H
#define SRC_SCREENS_LOGIN_SCREEN_H
#include <gtk/gtk.h>
void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_selectHomeserver_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_init();
#endif
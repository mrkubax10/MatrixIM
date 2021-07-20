#ifndef SRC_SCREENS_LOGIN_SCREEN_H
#define SRC_SCREENS_LOGIN_SCREEN_H
#include <gtk/gtk.h>
typedef struct LoginScreen{
    GtkWidget* fixedContainer;
    GtkWidget* homeserverContainer;
    GtkWidget* labelHomeserver;
    GtkWidget* entryHomeserver;
    GtkWidget* entryPort;
    GtkWidget* userDataContainer;
    GtkWidget* entryUsernameContainer;
    GtkWidget* entryUsername;
    GtkWidget* entryPasswordContainer;
    GtkWidget* entryPassword;
    GtkWidget* labelUsername;
    GtkWidget* labelPassword;
    GtkWidget* loginRegisterContainer;
    GtkWidget* buttonLogin;
    GtkWidget* buttonRegister;
} LoginScreen;
LoginScreen* LoginScreen_new();

extern LoginScreen* loginScreen;
void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_selectHomeserver_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_init();
#endif
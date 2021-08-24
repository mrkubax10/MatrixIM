#ifndef SRC_SCREENS_LOGIN_SCREEN_H
#define SRC_SCREENS_LOGIN_SCREEN_H
#include <gtk/gtk.h>
#include <stdbool.h>
typedef struct LoginScreen{
    GtkWidget* containerContent;
    GtkWidget* entryHomeserver;
    GtkWidget* buttonSelectHomeserver;
    GtkWidget* entryPort;
    GtkWidget* userDataContainer;
    GtkWidget* entryUsernameContainer;
    GtkWidget* entryUsername;
    GtkWidget* entryPasswordContainer;
    GtkWidget* entryPassword;
    GtkWidget* checkboxShowPassword;
    GtkWidget* loginRegisterContainer;
    GtkWidget* buttonLogin;
    GtkWidget* buttonRegister;
    char* lastSelectedHomeserver;
} LoginScreen;
LoginScreen* LoginScreen_new();
extern LoginScreen* loginScreen;
void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_buttonRegister_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_buttonHomeserverSelect_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_showPassword_toggle(GtkWidget* widget,gpointer userData);
void loginscreen_init();
void loginscreen_finish();
bool loginscreen_selectHomeserver(char* ip,int port);
#endif

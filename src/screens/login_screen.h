#ifndef SRC_SCREENS_LOGIN_SCREEN_H
#define SRC_SCREENS_LOGIN_SCREEN_H
#include <gtk/gtk.h>
#include <stdbool.h>
typedef struct LoginScreen{
    GtkWidget* containerContent;
    GtkWidget* labelHomeserver;
    GtkWidget* entryHomeserver;
    GtkWidget* buttonSelectHomeserver;
    GtkWidget* entryPort;
    GtkWidget* userDataContainer;
    GtkWidget* entryUsernameContainer;
    GtkWidget* entryUsername;
    GtkWidget* entryPasswordContainer;
    GtkWidget* entryPassword;
    GtkWidget* labelUsername;
    GtkWidget* labelPassword;
    GtkWidget* checkboxShowPassword;
    GtkWidget* loginRegisterContainer;
    GtkWidget* buttonLogin;
    GtkWidget* buttonRegister;
} LoginScreen;
LoginScreen* LoginScreen_new();
extern LoginScreen* loginScreen;
typedef enum LoginResult{
    LOGINRESULT_SUCCESS,LOGINRESULT_INTERNAL_ERROR,LOGINRESULT_SERVER_ERROR,LOGINRESULT_NETWORK_ERROR,LOGINRESULT_WRONG_USERNAME,LOGINRESULT_WRONG_PASSWORD,
    LOGINRESULT_UNSUPPORTED_LOGIN_TYPE,LOGINRESULT_UNSUPPORTED_MATRIX_REVISION,LOGINRESULT_WRONG_USERNAME_OR_PASSWORD
} LoginResult;
typedef enum RegisterResult{
    REGISTERRESULT_SUCCESS,REGISTERRESULT_INTERNAL_ERROR,REGISTERRESULT_NETWORK_ERROR,REGISTERRESULT_USERNAME_INVALID,
    REGISTERRESULT_SERVER_ERROR,REGISTERRESULT_WEAK_PASSWORD,REGISTERRESULT_UNKNOWN_ERROR,REGISTERRESULT_UNSUPPORTED_AUTH,
    REGISTERRESULT_WRONG_DATATYPE
} RegisterResult;
void loginscreen_buttonLogin_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_buttonRegister_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_selectHomeserver_clicked(GtkWidget* widget,gpointer userData);
void loginscreen_showPassword_toggle(GtkWidget* widget,gpointer userData);
void loginscreen_init();
void loginscreen_finish();
void loginscreen_checkLoginResult(LoginResult loginResult);
LoginResult loginscreen_login(char* ip,int port,char* username,char* password);
void loginscreen_checkRegisterResult(RegisterResult registerResult);
RegisterResult loginscreen_register(char* ip,int port,char* username,char* password);
#endif
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

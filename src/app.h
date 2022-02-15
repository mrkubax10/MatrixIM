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

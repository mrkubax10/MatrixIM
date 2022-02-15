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

#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include "app.h"
#include "network/socket.h"
#include "http/http.h"
#include "utils/filesystem.h"
#include "translation/translation.h"
int main(int argc,char** args){
    gtk_init(&argc,&args);
    srand(time(0));
    app=Application_new();
    ApplicationSettings_load(app->settings);
    GtkApplication* application=gtk_application_new("pl.mrkubax10.matrixim",G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application,"activate",G_CALLBACK(application_activate),0);
    g_application_run(G_APPLICATION(application),argc,args);
    g_object_unref(application);
    printf("(Log) [Main] Disconnecting from server\n");
    Socket_close(app->homeserverSocket);
    ApplicationSettings_save(app->settings);
    return 0;
}

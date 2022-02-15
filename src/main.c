#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "app.h"
#include "network/socket.h"
#include "http/http.h"
#include "utils/filesystem.h"
#include "utils.h"
#include "translation/translation.h"
int main(int argc,char** args){
    gtk_init(&argc,&args);
    srand(time(0));
    translation_setLanguage("pl");
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

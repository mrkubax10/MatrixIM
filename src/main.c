#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "app.h"
#include "network/socket.h"
#include "http/http.h"
int main(int argc,char** args){
    Socket* sock=Socket_new();
    Socket_connect(sock,"www.google.com",8080);
    http_sendGETRequest("/",sock);
    char result[2048];
    Socket_read(sock,result,2048);
    printf("%s",result);
    Socket_close(sock);
    //HTTPResponseInfo* info=http_parseResponse("HTTP/1.1 200 OK\nContent-Type: application/json\n\r\n{\"data\":\"val\"}");
    //printf("Version: %s\nCode: %d\nData Type: %s\nData: %s\n",info->version,info->code,info->datatype,info->data);
    app=(Application*)malloc(sizeof(Application));
    GtkApplication* application=gtk_application_new("pl.mrkubax10.matrixim",G_APPLICATION_FLAGS_NONE);
    g_signal_connect(application,"activate",G_CALLBACK(application_activate),0);
    g_application_run(G_APPLICATION(application),argc,args);
    g_object_unref(application);
    return 0;
}
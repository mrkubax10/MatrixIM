#include "screens/main_screen.h"
#include <stdlib.h>
#include "app.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include "app.h"
#include "http/http.h"
#include "screens/login_screen.h"
#include <cjson/cJSON.h>
MainScreen* MainScreen_new(){
    MainScreen* output=(MainScreen*)malloc(sizeof(MainScreen));
    return output;
}
MainScreen* mainScreen;
void mainscreen_messageEntry_key(GtkWidget* widget,gpointer data){
    unsigned int key;
    if(key==65293){ // Enter
        char* msg=(char*)gtk_entry_get_text(GTK_ENTRY(mainScreen->messageEntry));
        if(strcmp(msg,"")!=0)
            mainscreen_sendMessage(msg);
    }
}
void mainscreen_menuFileQuit_activated(GtkWidget* widget,gpointer userData){
    mainscreen_finish();
    gtk_window_close(GTK_WINDOW(app->window));
}
void mainscreen_menuFileLogout_activated(GtkWidget* widget,gpointer userData){
    if(!mainscreen_logout())
        return;
    mainscreen_finish();
    loginscreen_init();
}
void mainscreen_menuMatrixJoinRoom_activated(GtkWidget* widget,gpointer userData){
    char* room=mainscreen_showInputDialog("Room alias (for example #room:homeserver)");
    if(room!=0){
        int pathLength=snprintf(0,0,"/_matrix/client/r0/directory/room/%s",room);
        char* path=(char*)malloc(pathLength+1);
        // Obtain room ID from room alias
        snprintf(path,pathLength+1,"/_matrix/client/r0/directory/room/%s",room);
        http_sendGETRequest(path,app->loginInfo->homeserverName,app->homeserverSocket);
        free(path);
        char responseData[4096];
        Socket_read(app->homeserverSocket,responseData,4096);
        HTTPResponseInfo* response=http_parseResponse(responseData);
        if(response->code!=HTTP_CODE_OK){
            cJSON* jsonData=cJSON_Parse(response->data);
            char* errorMsg=(char*)malloc(strlen("Failed to enter room")+1);
            strcpy(errorMsg,"Failed to enter room");
            printf("%s\n",response->data);
            if(jsonData){
                cJSON* error=cJSON_GetObjectItemCaseSensitive(jsonData,"error");
                if(error){
                    int length=snprintf(0,0,"Failed to enter room: %s",error->valuestring);
                    free(errorMsg);
                    errorMsg=(char*)malloc(length+1);
                    snprintf(errorMsg,length+1,"Failed to enter room: %s",error->valuestring);
                    cJSON_free((void*)error);
                }
                cJSON_free((void*)jsonData);
            }
            mainscreen_showErrorDialog(errorMsg);
            HTTPResponseInfo_destroy(response);
            free(errorMsg);
            return;
        }
        cJSON* jsonData=cJSON_Parse(response->data);
        if(!jsonData){
            mainscreen_showErrorDialog("Failed to parse response while entering room");
            HTTPResponseInfo_destroy(response);
            return;
        }
        cJSON* roomID=cJSON_GetObjectItemCaseSensitive(jsonData,"room_id");
        if(!roomID){
            mainscreen_showErrorDialog("Failed to parse response while entering room");
            HTTPResponseInfo_destroy(response);
            cJSON_free((void*)jsonData);
            return;
        }
        HTTPResponseInfo_destroy(response);
        pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/join",roomID->valuestring);
        path=(char*)malloc(pathLength+1);
        snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/join",roomID->valuestring);
        http_sendPOSTRequest(path,app->loginInfo->homeserverName,"application/json",0,"",app->loginInfo->accessToken,app->homeserverSocket);
        cJSON_free((void*)roomID);
        cJSON_free((void*)jsonData);
        free(path);
        Socket_read(app->homeserverSocket,responseData,4096);
        response=http_parseResponse(responseData);
        if(response->code!=HTTP_CODE_OK){
            mainscreen_showErrorDialog("Failed to enter room");
            HTTPResponseInfo_destroy(response);
            return;
        }
        HTTPResponseInfo_destroy(response);
        GtkTreeIter iter;
        gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0);
        gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0,room,-1);
    }
}
void mainscreen_sendMessage(char* msg){
    //TODO
}
bool mainscreen_logout(){
    char* logoutRequest=matrix_createLogoutRequest(app->loginInfo->accessToken);
    printf("%s\n",logoutRequest);
    http_sendPOSTRequest("/_matrix/client/r0/logout",app->loginInfo->homeserverName,"",0,"",0,app->homeserverSocket);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Failed to logout");
        gtk_dialog_run(GTK_DIALOG(message));
        gtk_widget_destroy(message);
        HTTPResponseInfo_destroy(response);
        return false;
    }
    free(app->loginInfo->accessToken);
    free(app->loginInfo->deviceID);
    app->loginInfo->deviceID=0;
    app->settings->deviceID=0;
    app->loginInfo->accessToken=0;
    app->loggedIn=false;
    app->settings->wasLoggedIn=false;
    return true;
}
void mainscreen_init(){
    mainScreen=MainScreen_new();
    mainScreen->containerContent=gtk_grid_new();
    mainScreen->containerSidebar=gtk_grid_new();
    mainScreen->listFriends=gtk_tree_view_new();
    mainScreen->listRooms=gtk_tree_view_new();
    mainScreen->chat=gtk_text_view_new();
    mainScreen->messageEntry=gtk_entry_new();
    mainScreen->menuBar=gtk_menu_bar_new();
    mainScreen->menuItemFile=gtk_menu_item_new_with_label("File");
    mainScreen->menuItemMatrix=gtk_menu_item_new_with_label("Matrix");
    mainScreen->menuItemHelp=gtk_menu_item_new_with_label("Help");
    mainScreen->fileMenu=gtk_menu_new();
    mainScreen->fileQuit=gtk_menu_item_new_with_label("Quit");
    mainScreen->fileLogout=gtk_menu_item_new_with_label("Logout");
    mainScreen->matrixMenu=gtk_menu_new();
    mainScreen->matrixJoinRoom=gtk_menu_item_new_with_label("Join room");
    mainScreen->matrixLeaveRoom=gtk_menu_item_new_with_label("Leave room");
    mainScreen->matrixAddFriend=gtk_menu_item_new_with_label("Add friend");
    mainScreen->helpMenu=gtk_menu_new();
    mainScreen->helpAbout=gtk_menu_item_new_with_label("About");
    mainScreen->listRoomsStore=gtk_tree_store_new(1,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mainScreen->listRooms),GTK_TREE_MODEL(mainScreen->listRoomsStore));
    GtkCellRenderer* cellRenderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn* treeViewColumnRooms=gtk_tree_view_column_new_with_attributes("Rooms",cellRenderer,"text",0,0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listRooms),treeViewColumnRooms);
    mainScreen->listFriendsStore=gtk_tree_store_new(1,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mainScreen->listFriends),GTK_TREE_MODEL(mainScreen->listFriendsStore));
    GtkTreeViewColumn* treeViewColumnFriends=gtk_tree_view_column_new_with_attributes("Friends",cellRenderer,"text",0,0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listFriends),treeViewColumnFriends);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(mainScreen->chat),false);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(mainScreen->chat),GTK_WRAP_WORD);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->menuBar),mainScreen->menuItemFile);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->menuBar),mainScreen->menuItemMatrix);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->menuBar),mainScreen->menuItemHelp);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainScreen->menuItemFile),mainScreen->fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->fileMenu),mainScreen->fileLogout);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->fileMenu),mainScreen->fileQuit);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainScreen->menuItemMatrix),mainScreen->matrixMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->matrixMenu),mainScreen->matrixJoinRoom);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->matrixMenu),mainScreen->matrixLeaveRoom);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->matrixMenu),mainScreen->matrixAddFriend);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainScreen->menuItemHelp),mainScreen->helpMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(mainScreen->helpMenu),mainScreen->helpAbout);

    g_signal_connect(mainScreen->messageEntry,"key-press-event",mainscreen_messageEntry_key,0);
    g_signal_connect(mainScreen->fileLogout,"activate",mainscreen_menuFileLogout_activated,0);
    g_signal_connect(mainScreen->fileQuit,"activate",mainscreen_menuFileQuit_activated,0);
    g_signal_connect(mainScreen->matrixJoinRoom,"activate",mainscreen_menuMatrixJoinRoom_activated,0);

    gtk_fixed_put(GTK_FIXED(app->fixedContainer),mainScreen->containerContent,0,0);
    gtk_grid_attach(GTK_GRID(mainScreen->containerContent),mainScreen->menuBar,0,0,64,2);
    gtk_grid_attach(GTK_GRID(mainScreen->containerContent),mainScreen->containerSidebar,0,2,8,16);
    gtk_grid_attach(GTK_GRID(mainScreen->containerSidebar),mainScreen->listFriends,0,0,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerSidebar),mainScreen->listRooms,mainScreen->listFriends,GTK_POS_BOTTOM,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->messageEntry,mainScreen->containerSidebar,GTK_POS_BOTTOM,20,5);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->chat,mainScreen->containerSidebar,GTK_POS_RIGHT,16,16);
    gtk_widget_show_all(app->window);
}
void mainscreen_finish(){
    gtk_container_foreach(GTK_CONTAINER(app->fixedContainer),(GtkCallback)gtk_widget_destroy,0);
    free(mainScreen);
}
char* mainscreen_showInputDialog(char* msg){
    GtkWidget* dialog=gtk_dialog_new_with_buttons(msg,GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,"OK",GTK_RESPONSE_ACCEPT,"Cancel",GTK_RESPONSE_CANCEL,0);
    GtkWidget* contentArea=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* entry=gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(contentArea),entry);
    gtk_widget_show_all(dialog);
    int res=gtk_dialog_run(GTK_DIALOG(dialog));
    char* text=(char*)gtk_entry_get_text(GTK_ENTRY(entry));
    char* output=(char*)malloc(strlen(text)+1);
    strcpy(output,text);
    gtk_widget_destroy(dialog);
    return (res==-3?output:0); //-3 is OK button ID
}
void mainscreen_showErrorDialog(char* msg){
    GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,msg);
    gtk_dialog_run(GTK_DIALOG(message));
    gtk_widget_destroy(message);
}
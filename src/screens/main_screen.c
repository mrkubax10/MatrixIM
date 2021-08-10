#include "screens/main_screen.h"
#include <stdlib.h>
#include "app.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include "app.h"
#include "http/http.h"
#include "screens/login_screen.h"
#include <cjson/cJSON.h>
#include "utils/message.h"
#include <pthread.h>
MainScreen* MainScreen_new(){
    MainScreen* output=(MainScreen*)malloc(sizeof(MainScreen));
    output->enteredRooms=Vector_new();
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
    char* room=showInputDialog("Room alias (for example #room:homeserver)");
    if(room!=0){
        char* roomID=matrix_joinRoom(room);
        if(!roomID)
            return;
        GtkTreeIter iter;
        gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0);
        gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0,room,-1);
        MatrixRoom* matrixRoom=MatrixRoom_new();
        matrixRoom->roomAlias=room;
        matrixRoom->roomID=malloc(strlen(roomID)+1);
        strcpy(matrixRoom->roomID,roomID);
        free(roomID);
        Vector_push(mainScreen->enteredRooms,(void*)matrixRoom);
    }
}
void mainscreen_menuMatrixLeaveRoom_activated(GtkWidget* widget,gpointer userData){
    int res=showYesNoDialog("Are you sure?");
    if(res==-8){
        // FIXME: Find better way to do this
        GtkTreeSelection* selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(mainScreen->listRooms));
        gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
        GtkTreeModel* listRoomsModel=GTK_TREE_MODEL(mainScreen->listRoomsStore);
        GList* selectedRows=gtk_tree_selection_get_selected_rows(selection,&listRoomsModel);
        if(!selectedRows){
            showInfoDialog("You need to select room to leave");
            return;
        }
        GtkTreePath* treePath=selectedRows->data;
        int selectionID=atoi(gtk_tree_path_to_string(treePath));

        if(!matrix_leaveRoom(((MatrixRoom*)(mainScreen->enteredRooms->data[selectionID]))->roomID))
            return;
        GtkTreeIter iter;
        gtk_tree_model_get_iter(listRoomsModel,&iter,treePath);
        gtk_tree_store_remove(mainScreen->listRoomsStore,&iter);
        Vector_delete(mainScreen->enteredRooms,selectionID);
    }
}
void mainscreen_menuHelpAbout_activated(GtkWidget* widget,gpointer userdata){
    gtk_show_about_dialog(GTK_WINDOW(app->window),
        "name","MatrixIM - free and open-source Matrix client",
        "version","git",
        "copyright","2021 (C) mrkubax10",
        "logo-icon-name","gtk-about",
        0);
}
void mainscreen_sendMessage(char* msg){
    //TODO
}
void mainscreen_synchronizeEnteredRooms(){
    http_sendGETRequest("/_matrix/client/r0/joined_rooms",app->loginInfo->homeserverName,app->homeserverSocket,app->loginInfo->accessToken);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        showErrorDialog("Failed to synchronize entered rooms");
        mainscreen_finish();
        loginscreen_init();
        return;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog("Failed to parse entered rooms data");
        mainscreen_finish();
        loginscreen_init();
        return;
    }
    cJSON* enteredRooms=cJSON_GetObjectItemCaseSensitive(jsonData,"joined_rooms");
    if(!cJSON_IsArray(enteredRooms)){
        showErrorDialog("Failed to parse entered rooms data");
        mainscreen_finish();
        loginscreen_init();
        return;
    }
    cJSON* room=0;
    for(int i=0; (room=cJSON_GetArrayItem(enteredRooms,i)); i++){
        int pathLength=snprintf(0,0,"/_matrix/client/r0/rooms/%s/aliases",room->valuestring);
        char* path=malloc(pathLength+1);
        snprintf(path,pathLength+1,"/_matrix/client/r0/rooms/%s/aliases",room->valuestring);
        char* url=http_toHTTPURL(path);
        http_sendGETRequest(url,app->loginInfo->homeserverName,app->homeserverSocket,app->loginInfo->accessToken);
        free(path);
        free(url);
        Socket_read(app->homeserverSocket,responseData,4096);
        HTTPResponseInfo* responseInfo=http_parseResponse(responseData);
        cJSON* roomAliasesData=cJSON_Parse(responseInfo->data);
        if(!roomAliasesData){
            HTTPResponseInfo_destroy(responseInfo);
            continue;
        }
        cJSON* roomAliases=cJSON_GetObjectItemCaseSensitive(roomAliasesData,"aliases");
        if(!roomAliases){
            HTTPResponseInfo_destroy(responseInfo);
            cJSON_free((void*)roomAliasesData);
            continue;
        }
        cJSON* roomAlias=cJSON_GetArrayItem(roomAliases,0);
        GtkTreeIter iter;
        gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0);
        gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0,roomAlias->valuestring,-1);
        MatrixRoom* matrixRoom=MatrixRoom_new();
        matrixRoom->roomAlias=malloc(strlen(roomAlias->valuestring)+1);
        strcpy(matrixRoom->roomAlias,roomAlias->valuestring);
        matrixRoom->roomID=malloc(strlen(room->valuestring)+1);
        strcpy(matrixRoom->roomID,room->valuestring);
        Vector_push(mainScreen->enteredRooms,(void*)matrixRoom);
        HTTPResponseInfo_destroy(responseInfo);
        cJSON_free((void*)roomAlias);
        cJSON_free((void*)roomAliases);
        cJSON_free((void*)roomAliasesData);
    }
    cJSON_free((void*)enteredRooms);
    cJSON_free((void*)jsonData);
    HTTPResponseInfo_destroy(response);
}
void mainscreen_eventListener(void* data){
    //char message[4096];
    //while(mainScreen){
    //    printf("Waiting for message\n");
    //    Socket_read(app->homeserverSocket,message,4096);
    //    printf("%s\n",message);
    //}
}
bool mainscreen_logout(){
    http_sendPOSTRequest("/_matrix/client/r0/logout",app->loginInfo->homeserverName,"",0,"",app->loginInfo->accessToken,app->homeserverSocket);
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
    g_signal_connect(mainScreen->matrixLeaveRoom,"activate",mainscreen_menuMatrixLeaveRoom_activated,0);
    g_signal_connect(mainScreen->helpAbout,"activate",mainscreen_menuHelpAbout_activated,0);

    gtk_fixed_put(GTK_FIXED(app->fixedContainer),mainScreen->containerContent,0,0);
    gtk_grid_attach(GTK_GRID(mainScreen->containerContent),mainScreen->menuBar,0,0,64,2);
    gtk_grid_attach(GTK_GRID(mainScreen->containerContent),mainScreen->containerSidebar,0,2,8,16);
    gtk_grid_attach(GTK_GRID(mainScreen->containerSidebar),mainScreen->listFriends,0,0,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerSidebar),mainScreen->listRooms,mainScreen->listFriends,GTK_POS_BOTTOM,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->messageEntry,mainScreen->containerSidebar,GTK_POS_BOTTOM,20,5);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->chat,mainScreen->containerSidebar,GTK_POS_RIGHT,16,16);
    gtk_widget_show_all(app->window);
    mainscreen_synchronizeEnteredRooms();
    pthread_create(&mainScreen->matrixEventThread,0,mainscreen_eventListener,0);
}
void mainscreen_finish(){
    gtk_container_foreach(GTK_CONTAINER(app->fixedContainer),(GtkCallback)gtk_widget_destroy,0);
    free(mainScreen);
    mainScreen=0;
}
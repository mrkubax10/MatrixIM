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

#include "screens/main_screen.h"

#include <stdlib.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <cjson/cJSON.h>

#include "app.h"
#include "http/http.h"
#include "screens/login_screen.h"
#include "screens/new_room_dialog.h"
#include "utils/message.h"
#include "translation/translation.h"
MainScreen* MainScreen_new(){
    MainScreen* output=(MainScreen*)malloc(sizeof(MainScreen));
    output->enteredRooms=Vector_new();
    return output;
}
MainScreen* mainScreen;
void mainscreen_messageEntry_key(GtkWidget* widget,GdkEvent* event,gpointer data){
    unsigned int key;
    gdk_event_get_keyval(event,&key);
    
    if(key==65293){ // Enter
        char* msg=(char*)gtk_entry_get_text(GTK_ENTRY(mainScreen->messageEntry));
        if(strcmp(msg,"")==0)
            return;
        GtkTreeSelection* selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(mainScreen->listRooms));
        gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
        GtkTreeModel* listRoomsModel=GTK_TREE_MODEL(mainScreen->listRoomsStore);
        GList* selectedRows=gtk_tree_selection_get_selected_rows(selection,&listRoomsModel);
        if(!selectedRows){
            showInfoDialog(_("You need to select room to send message"));
            return;
        }
        GtkTreePath* treePath=selectedRows->data;
        int selectionID=atoi(gtk_tree_path_to_string(treePath));
        GtkTextBuffer* buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(mainScreen->chat));
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer,&iter);
        if(matrix_sendMessage(msg,((MatrixRoom*)mainScreen->enteredRooms->data[selectionID])->roomID)){
            char* fullMsg=malloc(strlen(app->settings->lastUsername)+strlen(msg)+3);
            memcpy(fullMsg,app->settings->lastUsername,strlen(app->settings->lastUsername));
            strcat(fullMsg,": ");
            memcpy(fullMsg+strlen(app->settings->lastUsername)+2,msg,strlen(msg));
            fullMsg[strlen(app->settings->lastUsername)+strlen(msg)+2]=0;
            gtk_text_buffer_insert(buffer,&iter,fullMsg,strlen(fullMsg));
            free(fullMsg);
        }
        else{
            gtk_text_buffer_insert(buffer,&iter,_("Failed to send message"),strlen(_("Failed to send message")));
        }
        gtk_text_buffer_insert(buffer,&iter,"\n",1);
        gtk_entry_set_text(GTK_ENTRY(mainScreen->messageEntry),"");
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
    char* room=showInputDialog(_("Room alias (for example #room:homeserver)"));
    if(room!=0){
        char* roomID=matrix_joinRoom(room);
        if(!roomID)
            return;
        GtkTreeIter iter;
        gtk_list_store_append(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter);
        gtk_list_store_set(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter,0,room,-1);
        MatrixRoom* matrixRoom=MatrixRoom_new();
        matrixRoom->roomAlias=room;
        matrixRoom->roomID=malloc(strlen(roomID)+1);
        strcpy(matrixRoom->roomID,roomID);
        free(roomID);
        Vector_push(mainScreen->enteredRooms,(void*)matrixRoom);
    }
}
void mainscreen_menuMatrixLeaveRoom_activated(GtkWidget* widget,gpointer userData){
    int res=showYesNoDialog(_("Are you sure?"));
    if(res==-8){
        GtkTreeSelection* selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(mainScreen->listRooms));
        gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
        GtkTreeModel* listRoomsModel=GTK_TREE_MODEL(mainScreen->listRoomsStore);
        GList* selectedRows=gtk_tree_selection_get_selected_rows(selection,&listRoomsModel);
        if(!selectedRows){
            showInfoDialog(_("You need to select room to leave"));
            return;
        }
        GtkTreePath* treePath=selectedRows->data;
        int selectionID=atoi(gtk_tree_path_to_string(treePath));
        if(!matrix_leaveRoom(((MatrixRoom*)(mainScreen->enteredRooms->data[selectionID]))->roomID))
            return;
        GtkTreeIter iter;
        gtk_tree_model_get_iter(listRoomsModel,&iter,treePath);
        gtk_list_store_remove(mainScreen->listRoomsStore,&iter);
        Vector_delete(mainScreen->enteredRooms,selectionID);
    }
}
void mainscreen_menuMatrixCreateRoom_activated(GtkWidget* widget,gpointer userData){
    newroomdialog_init();
}
void mainscreen_menuHelpAbout_activated(GtkWidget* widget,gpointer userData){
    gtk_show_about_dialog(GTK_WINDOW(app->window),
        "name",_("MatrixIM - free and open-source Matrix client"),
        "version","git",
        "copyright","2021-2022 (C) mrkubax10",
        "logo-icon-name","gtk-about",
        NULL);
}
void mainscreen_listRooms_rowChanged(GtkWidget* widget,gpointer userData){
    int selectionID=mainscreen_getSelectedRoom();
    if(selectionID==-1 || selectionID>mainScreen->enteredRooms->capacity)
        return;
    MatrixRoom* room=mainScreen->enteredRooms->data[selectionID];
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(mainScreen->chat),room->chatBuffer);
}
int mainscreen_getSelectedRoom(){
    // FIXME: Find better way to do this
    GtkTreeSelection* selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(mainScreen->listRooms));
    gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
    GtkTreeModel* listRoomsModel=GTK_TREE_MODEL(mainScreen->listRoomsStore);
    GList* selectedRows=gtk_tree_selection_get_selected_rows(selection,&listRoomsModel);
    if(!selectedRows)
        return -1;
    GtkTreePath* treePath=selectedRows->data;
    int selectionID=atoi(gtk_tree_path_to_string(treePath));
    return selectionID;
}
void mainscreen_synchronizeEnteredRooms(){
    http_sendGETRequest("/_matrix/client/r0/joined_rooms",app->loginInfo->homeserverName,app->homeserverSocket,app->loginInfo->accessToken);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        showErrorDialog(_("Failed to synchronize entered rooms"));
        return;
    }
    cJSON* jsonData=cJSON_Parse(response->data);
    if(!jsonData){
        showErrorDialog(_("Failed to parse entered rooms data"));
        mainscreen_finish();
        loginscreen_init();
        return;
    }
    cJSON* enteredRooms=cJSON_GetObjectItemCaseSensitive(jsonData,"joined_rooms");
    if(!cJSON_IsArray(enteredRooms)){
        showErrorDialog(_("Failed to parse entered rooms data"));
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
        gtk_list_store_append(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter);
        gtk_list_store_set(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter,0,roomAlias->valuestring,-1);
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
    char message[4096];
    while(mainScreen){
       printf("Waiting for message\n");
       Socket_read(app->homeserverSocket,message,4096);
       printf("%s\n",message);
    }
}
bool mainscreen_logout(){
    http_sendPOSTRequest("/_matrix/client/r0/logout",app->loginInfo->homeserverName,"",0,"",app->loginInfo->accessToken,app->homeserverSocket);
    char responseData[4096];
    Socket_read(app->homeserverSocket,responseData,4096);
    HTTPResponseInfo* response=http_parseResponse(responseData);
    if(response->code!=HTTP_CODE_OK){
        GtkWidget* message=gtk_message_dialog_new(GTK_WINDOW(app->window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,_("Failed to logout"));
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
    GtkBuilder* builder=gtk_builder_new();
    if(!gtk_builder_add_from_file(GTK_BUILDER(builder),"ui/matrixim_main_screen.ui",0)){
        showErrorDialog(_("Failed to load UI file matrixim_main_screen.ui"));
        exit(0);
    }
    gtk_builder_connect_signals(builder,0);
    mainScreen->containerContent=GTK_WIDGET(gtk_builder_get_object(builder,"containerContent"));
    gtk_container_add(GTK_CONTAINER(app->window),mainScreen->containerContent);
    mainScreen->listFriends=GTK_WIDGET(gtk_builder_get_object(builder,"listFriends"));
    mainScreen->listRooms=GTK_WIDGET(gtk_builder_get_object(builder,"listRooms"));
    mainScreen->chat=GTK_WIDGET(gtk_builder_get_object(builder,"chat"));
    mainScreen->messageEntry=GTK_WIDGET(gtk_builder_get_object(builder,"entryMessage"));
    mainScreen->listRoomsStore=GTK_LIST_STORE(gtk_builder_get_object(builder,"listRoomsStore"));
    GtkCellRenderer* cellRenderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn* treeViewColumnRooms=gtk_tree_view_column_new_with_attributes("Rooms",cellRenderer,"text",0,NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listRooms),treeViewColumnRooms);
    mainScreen->listFriendsStore=GTK_LIST_STORE(gtk_builder_get_object(builder,"listFriendsStore"));
    GtkTreeViewColumn* treeViewColumnFriends=gtk_tree_view_column_new_with_attributes("Friends",cellRenderer,"text",0,NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listFriends),treeViewColumnFriends);
    mainscreen_synchronizeEnteredRooms();
    gtk_widget_show_all(app->window);
    g_object_unref(builder);
    //pthread_create(mainScreen->matrixEventThread,0,mainscreen_eventListener,0);
}
void mainscreen_finish(){
    gtk_container_foreach(GTK_CONTAINER(app->fixedContainer),(GtkCallback)gtk_widget_destroy,0);
    free(mainScreen);
    mainScreen=0;
}
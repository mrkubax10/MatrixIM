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

#include "screens/new_room_dialog.h"

#include <stdlib.h>
#include <stdio.h>

#include "matrix/room.h"
#include "utils/message.h"
#include "utils/vector.h"
#include "screens/main_screen.h"
#include "translation/translation.h"
NewRoomDialog* newRoomDialog;
NewRoomDialog* NewRoomDialog_new(){
    NewRoomDialog* output=malloc(sizeof(NewRoomDialog));
    return output;
}
void newroomdialog_init(){
    newRoomDialog=NewRoomDialog_new();
    GtkBuilder* builder=gtk_builder_new();
    if(!gtk_builder_add_from_file(builder,"ui/matrixim_new_room_dialog.ui",0)){
        char msg[512];
        snprintf(msg,512,_("Failed to load UI file %s"),"matrixim_new_room_dialog.ui");
        showErrorDialog(msg);
        exit(0);
    }
    gtk_builder_connect_signals(builder,0);
    newRoomDialog->window=GTK_WIDGET(gtk_builder_get_object(builder,"window"));
    newRoomDialog->entryRoomAlias=GTK_WIDGET(gtk_builder_get_object(builder,"entryRoomAlias"));
    newRoomDialog->entryRoomName=GTK_WIDGET(gtk_builder_get_object(builder,"entryRoomName"));
    newRoomDialog->entryRoomTopic=GTK_WIDGET(gtk_builder_get_object(builder,"entryRoomTopic"));
    newRoomDialog->checkboxVisible=GTK_WIDGET(gtk_builder_get_object(builder,"checkboxVisible"));
    gtk_widget_show_all(newRoomDialog->window);
    g_object_unref(builder);
}
void newroomdialog_finish(){
    gtk_window_close(GTK_WINDOW(newRoomDialog->window));
    gtk_widget_destroy(newRoomDialog->window);
    free(newRoomDialog);
    newRoomDialog=0;
}
void newroomdialog_buttonCreate_activated(GtkWidget* widget,gpointer userData){
    char* alias=(char*)gtk_entry_get_text(GTK_ENTRY(newRoomDialog->entryRoomAlias));
    char* name=(char*)gtk_entry_get_text(GTK_ENTRY(newRoomDialog->entryRoomName));
    char* topic=(char*)gtk_entry_get_text(GTK_ENTRY(newRoomDialog->entryRoomTopic));
    if(strcmp(alias,"")==0){
        showInfoDialog(_("Alias cannot be empty"));
        return;
    }
    char* roomID=matrix_createRoom(alias,name,topic,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(newRoomDialog->checkboxVisible)));
    if(!roomID)
        return;
    MatrixRoom* room=MatrixRoom_new();
    room->roomAlias=matrix_getRoomAliasByID(roomID);
    if(!room->roomAlias){
        free(roomID);
        MatrixRoom_destroy(room);
        return;
    }
    room->roomID=roomID;
    Vector_push(mainScreen->enteredRooms,(void*)room);
    GtkTreeIter iter;
    gtk_list_store_append(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter);
    gtk_list_store_set(GTK_LIST_STORE(mainScreen->listRoomsStore),&iter,0,room->roomAlias,-1);
    newroomdialog_finish();
}
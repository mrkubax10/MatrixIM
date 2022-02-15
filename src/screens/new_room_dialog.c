#include "screens/new_room_dialog.h"
#include <stdlib.h>
#include "matrix/room.h"
#include "utils/message.h"
#include "utils/vector.h"
#include "screens/main_screen.h"
#include <stdio.h>
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
        showErrorDialog(_("Failed to load UI file matrixim_new_room_dialog.ui"));
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
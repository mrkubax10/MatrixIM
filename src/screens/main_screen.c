#include "screens/main_screen.h"
#include <stdlib.h>
#include "app.h"
#include <gtk/gtk.h>
#include <stdio.h>
MainScreen* MainScreen_new(){
    MainScreen* output=(MainScreen*)malloc(sizeof(MainScreen));
    return output;
}
MainScreen* mainScreen;
void mainscreen_messageEntry_key(GtkWidget* widget,gpointer data){
    unsigned int key;
    gdk_event_get_keyval(event,&key);
    printf("%d\n",key);
}
void mainscreen_init(){
    mainScreen=MainScreen_new();
    mainScreen->containerContent=gtk_grid_new();
    mainScreen->containerSidebar=gtk_grid_new();
    mainScreen->listFriends=gtk_tree_view_new();
    mainScreen->listRooms=gtk_tree_view_new();
    mainScreen->chat=gtk_text_view_new();
    mainScreen->messageEntry=gtk_entry_new();

    mainScreen->listRoomsStore=gtk_tree_store_new(1,G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0);
    gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0,"Test",-1);
    gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0);
    gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listRoomsStore),&iter,0,"Test",-1);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mainScreen->listRooms),GTK_TREE_MODEL(mainScreen->listRoomsStore));
    GtkCellRenderer* cellRenderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn* treeViewColumnRooms=gtk_tree_view_column_new_with_attributes("Rooms",cellRenderer,"text",0,0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listRooms),treeViewColumnRooms);
    mainScreen->listFriendsStore=gtk_tree_store_new(1,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mainScreen->listFriends),GTK_TREE_MODEL(mainScreen->listFriendsStore));
    gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listFriendsStore),&iter,0);
    gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listFriendsStore),&iter,0,"Test",-1);
    gtk_tree_store_append(GTK_TREE_STORE(mainScreen->listFriendsStore),&iter,0);
    gtk_tree_store_set(GTK_TREE_STORE(mainScreen->listFriendsStore),&iter,0,"Test",-1);
    GtkTreeViewColumn* treeViewColumnFriends=gtk_tree_view_column_new_with_attributes("Friends",cellRenderer,"text",0,0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(mainScreen->listFriends),treeViewColumnFriends);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(mainScreen->chat),false);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(mainScreen->chat),GTK_WRAP_WORD);

    g_signal_connect(mainScreen->messageEntry,"key-press-event",mainscreen_messageEntry_key,0);

    gtk_fixed_put(GTK_FIXED(app->fixedContainer),mainScreen->containerContent,0,0);
    gtk_grid_attach(GTK_GRID(mainScreen->containerContent),mainScreen->containerSidebar,0,0,8,16);
    gtk_grid_attach(GTK_GRID(mainScreen->containerSidebar),mainScreen->listFriends,0,0,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerSidebar),mainScreen->listRooms,mainScreen->listFriends,GTK_POS_BOTTOM,8,8);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->messageEntry,mainScreen->containerSidebar,GTK_POS_BOTTOM,20,5);
    gtk_grid_attach_next_to(GTK_GRID(mainScreen->containerContent),mainScreen->chat,mainScreen->containerSidebar,GTK_POS_RIGHT,16,16);
    gtk_widget_show_all(app->window);
}
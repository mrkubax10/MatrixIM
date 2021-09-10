#ifndef SRC_SCREENS_NEW_ROOM_DIALOG_H
#define SRC_SCREENS_NEW_ROOM_DIALOG_H
#include <gtk/gtk.h>
typedef struct NewRoomDialog{
    GtkWidget* window;
    GtkWidget* entryRoomAlias;
    GtkWidget* entryRoomName;
    GtkWidget* entryRoomTopic;
    GtkWidget* checkboxVisible;
} NewRoomDialog;
NewRoomDialog* NewRoomDialog_new();
extern NewRoomDialog* newRoomDialog;
void newroomdialog_init();
void newroomdialog_finish();
void newroomdialog_buttonCreate_activated(GtkWidget* widget,gpointer userData);
#endif
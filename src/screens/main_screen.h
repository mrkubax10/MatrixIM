#ifndef SRC_SCREENS_MAIN_SCREEN_H
#define SRC_SCREENS_MAIN_SCREEN_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "matrix/room.h"
#include "utils/vector.h"
typedef struct MainScreen{
    GtkWidget* containerSidebar;
    GtkWidget* containerContent;
    GtkWidget* listFriends;
    GtkWidget* listRooms;
    GtkWidget* chat;
    GtkWidget* messageEntry;
    GtkWidget* menuBar;
    GtkWidget* menuItemFile;
    GtkWidget* menuItemMatrix;
    GtkWidget* menuItemHelp;
    GtkWidget* fileMenu;
    GtkWidget* matrixMenu;
    GtkWidget* helpMenu;
    GtkWidget* fileLogout;
    GtkWidget* fileQuit;
    GtkWidget* matrixJoinRoom;
    GtkWidget* matrixLeaveRoom;
    GtkWidget* matrixAddFriend;
    GtkWidget* helpAbout;
    GtkTreeStore* listFriendsStore;
    GtkTreeStore* listRoomsStore;
    Vector* enteredRooms;
} MainScreen;
MainScreen* MainScreen_new();
extern MainScreen* mainScreen;
void mainscreen_messageEntry_key(GtkWidget* widget,gpointer userData);
void mainscreen_menuFileQuit_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuFileLogout_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuMatrixJoinRoom_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuMatrixLeaveRoom_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuHelpAbout_activated(GtkWidget* widget,gpointer userData);
void mainscreen_listRooms_changed(GtkWidget* widget,gpointer userData);
void mainscreen_sendMessage(char* msg);
void mainscreen_synchronizeEnteredRooms();
bool mainscreen_logout();
void mainscreen_init();
void mainscreen_finish();
char* mainscreen_showInputDialog(char* msg);
void mainscreen_showErrorDialog(char* msg);
#endif
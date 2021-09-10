#ifndef SRC_SCREENS_MAIN_SCREEN_H
#define SRC_SCREENS_MAIN_SCREEN_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "matrix/room.h"
#include "utils/vector.h"
typedef struct MainScreen{
    GtkWidget* containerContent;
    GtkWidget* listFriends;
    GtkWidget* listRooms;
    GtkWidget* chat;
    GtkWidget* messageEntry;
    GtkListStore* listFriendsStore;
    GtkListStore* listRoomsStore;
    Vector* enteredRooms;
    pthread_t* matrixEventThread;
    bool connectionLocked;
} MainScreen;
MainScreen* MainScreen_new();
extern MainScreen* mainScreen;
void mainscreen_messageEntry_key(GtkWidget* widget,GdkEvent* event,gpointer userData);
void mainscreen_menuFileQuit_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuFileLogout_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuMatrixJoinRoom_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuMatrixLeaveRoom_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuMatrixCreateRoom_activated(GtkWidget* widget,gpointer userData);
void mainscreen_menuHelpAbout_activated(GtkWidget* widget,gpointer userData);
void mainscreen_synchronizeEnteredRooms();
void mainscreen_eventListener(void* data);
bool mainscreen_logout();
void mainscreen_init();
void mainscreen_finish();
char* mainscreen_showInputDialog(char* msg);
void mainscreen_showErrorDialog(char* msg);
#endif
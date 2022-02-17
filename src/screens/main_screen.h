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
void mainscreen_listRooms_rowChanged(GtkWidget* widget,gpointer userData);
int mainscreen_getSelectedRoom();
void* mainscreen_synchronizeEnteredRooms(void* arg);
void mainscreen_eventListener(void* data);
bool mainscreen_logout();
void mainscreen_init();
void mainscreen_finish();
char* mainscreen_showInputDialog(char* msg);
void mainscreen_showErrorDialog(char* msg);
#endif
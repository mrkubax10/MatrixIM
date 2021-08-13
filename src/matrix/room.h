#ifndef SRC_MATRIX_ROOM_H
#define SRC_MATRIX_ROOM_H
#include <stdbool.h>
typedef struct MatrixRoom{
    char* roomID;
    char* roomAlias;
} MatrixRoom;
MatrixRoom* MatrixRoom_new();
char* matrix_joinRoom(char* room);
bool matrix_leaveRoom(char* roomID);
bool matrix_sendMessage(char* message,char* roomID);
#endif
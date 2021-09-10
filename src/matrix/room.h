#ifndef SRC_MATRIX_ROOM_H
#define SRC_MATRIX_ROOM_H
#include <stdbool.h>
typedef struct MatrixRoom{
    char* roomID;
    char* roomAlias;
} MatrixRoom;
MatrixRoom* MatrixRoom_new();
void MatrixRoom_destroy(MatrixRoom* room);
char* matrix_joinRoom(char* room);
bool matrix_joinRoomWithID(char* id);
bool matrix_leaveRoom(char* roomID);
bool matrix_sendMessage(char* message,char* roomID);
char* matrix_createRoom(char* alias,char* name,char* topic,bool visible);
char* matrix_getRoomAliasByID(char* id);
#endif
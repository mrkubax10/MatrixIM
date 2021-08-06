#ifndef SRC_MATRIX_ROOM_H
#define SRC_MATRIX_ROOM_H
typedef struct MatrixRoom{
    char* roomID;
    char* roomAlias;
} MatrixRoom;
MatrixRoom* MatrixRoom_new();
#endif
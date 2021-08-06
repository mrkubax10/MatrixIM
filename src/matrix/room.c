#include "matrix/room.h"
#include <stdlib.h>
#include <cjson/cJSON.h>
MatrixRoom* MatrixRoom_new(){
    MatrixRoom* output=(MatrixRoom*)malloc(sizeof(MatrixRoom));
    return output;
}
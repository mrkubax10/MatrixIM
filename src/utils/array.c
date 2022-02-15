#include "utils/array.h"

#include <stdlib.h>
void array_free(void** data,int len){
    for(int i=0; i<len; i++){
       free(data[i]);
    }
    free(data);
}
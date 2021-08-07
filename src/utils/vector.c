#include "utils/vector.h"
#include <stdlib.h>
Vector* Vector_new(){
    Vector* output=malloc(sizeof(Vector));
    output->capacity=0;
    output->data=0;
}
void Vector_push(Vector* vector,void* obj){
    vector->capacity++;
    vector->data=realloc(vector->data,vector->capacity);
    vector->data[vector->capacity-1]=obj;
}
void Vector_pop(Vector* vector){
    if(vector->capacity--<0){
        vector->capacity++;
        return;
    }
    vector->data=realloc(vector->data,vector->capacity);
}
void Vector_destroy(Vector* vector,bool destroyObjects){
    if(destroyObjects){
        for(unsigned int i=0; i<vector->capacity; i++){
            free(vector->data[i]);
        }
    }
    free(vector->data);
    free(vector);
}
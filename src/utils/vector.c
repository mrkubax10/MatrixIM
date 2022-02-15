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

#include "utils/vector.h"

#include <stdlib.h>
Vector* Vector_new(){
    Vector* output=malloc(sizeof(Vector));
    output->capacity=0;
    output->data=0;
    return output;
}
void Vector_push(Vector* vector,void* obj){
    vector->capacity++;
    vector->data=realloc(vector->data,vector->capacity*sizeof(void*));
    vector->data[vector->capacity-1]=obj;
}
void Vector_pop(Vector* vector){
    if(vector->capacity--<0){
        vector->capacity++;
        return;
    }
    vector->data=realloc(vector->data,vector->capacity*sizeof(void*));
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
void Vector_delete(Vector* vector,unsigned int index){
    if(index>=vector->capacity)
        return;
    if(index==vector->capacity-1){
        Vector_pop(vector);
        return;
    }
    for(int i=index+1; i<vector->capacity; i++){
        vector->data[i-1]=vector->data[i];
    }
    vector->capacity--;
    vector->data=realloc(vector->data,vector->capacity*sizeof(void*));
}
int Vector_find(Vector* vector,void* obj){
    for(unsigned int i=0; i<vector->capacity; i++){
        if(vector->data[i]==obj)
            return (int)i;
    }
    return -1;
}
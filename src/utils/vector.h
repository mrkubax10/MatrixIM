#ifndef SRC_UTILS_VECTOR_H
#define SRC_UTILS_VECTOR_H
#include <stdbool.h>
typedef struct Vector{
    void** data;
    unsigned int capacity;
} Vector;
Vector* Vector_new();
void Vector_push(Vector* vector,void* obj);
void Vector_pop(Vector* vector);
void Vector_destroy(Vector* vector,bool destroyObjects);
void Vector_delete(Vector* vector,unsigned int index);
int Vector_find(Vector* vector,void* obj);
#endif
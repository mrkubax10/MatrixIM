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
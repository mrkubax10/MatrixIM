#ifndef SRC_MATRIX_REGISTER_H
#define SRC_MATRIX_REGISTER_H
#include <stdbool.h>
char* matrix_createPasswordRegisterRequest(char* username,char* password,char* deviceName,char* deviceID,char* session);
bool matrix_registerPassword(char* ip,char* username,char* password);
#endif
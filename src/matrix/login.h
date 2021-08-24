#ifndef SRC_MATRIX_LOGIN_H
#define SRC_MATRIX_LOGIN_H
#include <stdbool.h>
typedef struct LoginInfo{
    char* userID;
    char* accessToken;
    char* homeserverName;
    char* deviceID;
} LoginInfo;
LoginInfo* LoginInfo_new();
char* matrix_createPasswordLoginRequest(char* username,char* password,char* deviceName,char* deviceID);
char* matrix_generateTnxID();
bool matrix_loginPassword(char* ip,char* username,char* password,char* deviceName,char* deviceID);
#endif
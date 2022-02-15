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
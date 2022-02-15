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

#ifndef SRC_MATRIX_HOMESERVER_H
#define SRC_MATRIX_HOMESERVER_H
#include <stdbool.h>
typedef struct HomeserverInfo{
    bool supportsLoginPassword;
    bool supportsLoginRecaptcha;
    bool supportsLoginOAuth2;
    bool supportsLoginSSO;
    bool supportsLoginEmailIdentity;
    bool supportsLoginMsisdn;
    bool supportsLoginToken;
    bool supportsLoginDummy;
} HomeserverInfo;
HomeserverInfo* HomeserverInfo_new();
void HomeserverInfo_destroy(HomeserverInfo* homeserver);
HomeserverInfo* matrix_getHomeserverInfo(char* ip,int port);

#endif

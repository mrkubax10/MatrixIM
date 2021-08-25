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

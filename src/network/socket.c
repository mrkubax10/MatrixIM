#include "network/socket.h"
Socket* Socket_new(){
    Socket* output=(Socket*)malloc(sizeof(Socket));
    output->connected=false;
    return output;
}
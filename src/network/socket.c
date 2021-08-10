#include "network/socket.h"
#include <stdlib.h>
Socket* Socket_new(){
    Socket* output=malloc(sizeof(Socket));
    output->connected=false;
    return output;
}
/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include"client_stub.h"
#include "message-private.h"

/* Remote table. A definir pelo grupo em client_stub-private.h
 */
struct rtable_t {
    char *hostname;
    unsigned short port;
    struct sockaddr_in *server_addr;
    int sockfd;
};

#endif

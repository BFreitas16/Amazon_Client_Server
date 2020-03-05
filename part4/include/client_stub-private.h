/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include"client_stub.h"
#include "message-private.h"

// typedef struct server {
//     char *ip;
//     unsigned short port;
// } server_t;

/* Remote table. A definir pelo grupo em client_stub-private.h
 */
struct rtable_t {
    char *hostname;
    unsigned short port;
    struct sockaddr_in *server_addr;
    int sockfd;
    char * znode; // node deste server
};

typedef struct rtable_t server_t;


/**
* Watcher function for connection state change events
*/
void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void* context);

/*
 * Deolve o id do node
 */
int getIdfromNode(char * node);

int findMinMaxNode(char ** min, char ** max, zoo_string* children_list);

char * getNodeData(char * node_name);

/*
Atualiza o head e tail recebidos na children list
caso tenham mudado deve ligar se aos novos servidores
*/
int updateHeadAndTail(zoo_string* children_list);

#endif

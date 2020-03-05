/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <zookeeper/zookeeper.h>
#include "sdmessage.pb-c.h"

// Tamanho máximo da mensagem enviada pelo cliente
#define MAX_MSG 2048
/* ZooKeeper Znode Data Length (1MB, the max supported) */
#define ZDATALEN 1024 * 1024

typedef struct String_vector zoo_string; 

// Estrutura mensagem
struct message_t
{
    MessageT *msg;
};

/*
 * Cria uma message passando-lhe os parametros que são relevantes
 */
struct message_t * make_message(MessageT__Opcode opcode, MessageT__CType ctype, int data_size, void *data, char * key, int32_t result, size_t n_keys, char **keys);

/*
 * Liberta toda a memoria de message. Message que não foi obtida
 * pelo metodo unpack
 */
void destroy_mensage_t_sent(struct message_t * message);

/*
 * Liberta toda a memoria de message. Message que foi obtida
 * pelo metodo unpack
 */
void destroy_mensage_t_received(struct message_t * message);

/*
 * Serializa mensagem e envia para a rede
 * Retorna 0 (OK) ou -1 (erro)
 */
int write_all(struct message_t * msg_to_send, int sockfd);

/*
 * Lê e deserializa a mensagem a ser recebida pela rede
 * Retorna a  mensagem recebida ou NULL em caso de erro.
 */
struct message_t * read_all(int sockfd);

#endif

/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#include "message-private.h"

/*
 * Fazer uma message passado-lhe os parametros que sao relevantes
 */
struct message_t * make_message(MessageT__Opcode opcode, MessageT__CType ctype, int data_size, void *data, char * key, int32_t result, size_t n_keys, char **keys)
{
    // alocar para message_t
    struct message_t *msg;
    msg = (struct message_t *) malloc(sizeof(struct message_t));
    if (msg == NULL) {
        perror("ERRO no malloc do make_message");
        return NULL;
    }

    // alocar para MessageT
    MessageT *m;
    m = (MessageT *) malloc(sizeof(MessageT));
    if (m == NULL) {
        perror("ERRO no malloc da MessageT no make_message");
        return NULL;
    }

    message_t__init(m);

    // copiar atributos
    m->opcode = opcode;
    m->c_type = ctype;
    m->key = key;
    m->data.len = data_size;
    m->data.data = data;
    m->result = result;
    m->n_keys = n_keys;
    m->keys = keys;

    msg->msg = m;

    return msg;
}

/*
 * Liberta toda a memoria de message. Message que não foi obtida
 * pelo metodo unpack
 */
void destroy_mensage_t_sent(struct message_t * message) {
    if(message != NULL) {
        free(message->msg);
    }
    free(message);
}

/*
 * Liberta toda a memoria de message. Message que foi obtida
 * pelo metodo unpack
 */
void destroy_mensage_t_received(struct message_t * message) {
    if(message != NULL) {
        message_t__free_unpacked(message->msg, NULL);
    }
    free(message);
}

/*
 * Serializa mensagem e envia para a rede
 * Retorna 0 (OK) ou -1 (erro)
 */
int write_all(struct message_t * msg_to_send, int sockfd) {

    if (msg_to_send == NULL) return -1;

    void *buf_to_send;

    // tamanho da mensagem
    size_t buf_len = message_t__get_packed_size(msg_to_send->msg);
    
    // alocar buff para enviar
    buf_to_send = malloc(buf_len);
    if (buf_to_send == NULL) {
        perror("ERRO malloc do network_send_receive");
        return -1;
    }

    message_t__pack(msg_to_send->msg, buf_to_send);

    // Enviar mensagem
    if(write(sockfd,buf_to_send,buf_len) != buf_len){
        perror("ERRO ao enviar dados");
        return -1;
    }

    free(buf_to_send);

    return 0;
}

/*
 * Lê e deserializa a mensagem a ser recebida pela rede
 * Retorna a  mensagem recebida ou NULL em caso de erro.
*/
struct message_t * read_all(int sockfd) {
    int nbytes;
    
    // alocar buffer para receber
    uint8_t *buf_to_receive = (uint8_t *) malloc(MAX_MSG);
    if (buf_to_receive == NULL) {
        perror("ERRO malloc de buf_to_receive");
        free(buf_to_receive);
        return NULL;
    }

    // Receber mensagem
    if((nbytes = read(sockfd,buf_to_receive,MAX_MSG)) < 0){
        perror("ERRO ao receber dados");
        free(buf_to_receive);
        return NULL;
    }

    // EOF
    if (nbytes == 0) {
        free(buf_to_receive);
        return NULL;
    }

    MessageT * msg = message_t__unpack(NULL, nbytes, buf_to_receive);
    if (msg == NULL) {
        perror("ERRO unpacking message\n");
        free(buf_to_receive);
        return NULL;
    }

    free(buf_to_receive);

    // alocar mensagem
    struct message_t *mensagem;
    mensagem = (struct message_t *) malloc(sizeof(struct message_t));
    if (mensagem == NULL) {
        perror("ERRO no malloc da message_t no read_all");
        return NULL;
    }

    mensagem->msg = msg;

    return mensagem;
}

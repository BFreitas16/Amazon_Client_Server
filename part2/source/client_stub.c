/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#include"client_stub-private.h"
#include "network_client.h"

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtable_t *rtable_connect(const char *address_port){

    if (address_port == NULL) return NULL;

    // copia do address_port
    char *address_port_copy = strdup(address_port);

    struct rtable_t *rtable;

    // reservar memoria

    rtable = (struct rtable_t *) malloc(sizeof(struct rtable_t));
    if (rtable == NULL) {
        free(address_port_copy);
        perror("ERRO no rtable malloc");
        return NULL;
    }

    rtable->server_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    if (rtable->server_addr == NULL) {
        free(rtable);
        free(address_port_copy);
        perror("ERRO no rtable->server_addr malloc");
        return NULL;
    }

    // tratar o address_port

    char *token;
    token = strtok(address_port_copy, ":");
    rtable->hostname = strdup(token);

    token = strtok(NULL, ":");
    if ((rtable->port = atoi(token)) == 0) {
        free(rtable->server_addr);
        free(rtable->hostname);
        free(rtable);
        free(address_port_copy);
        perror("ERRO porto inválido");
        return NULL;
    }

    // estabelecer conexao
    if (network_connect(rtable) == -1) {
        free(rtable->server_addr);
        free(rtable->hostname);
        free(rtable);
        free(address_port_copy);
        return NULL;
    }

    free(address_port_copy);

    return rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable){
    network_close(rtable);
    free(rtable->hostname);
    free(rtable->server_addr);
    free(rtable);
    return 0;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry) {

    if (rtable == NULL || entry == NULL) return -1;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_PUT, MESSAGE_T__C_TYPE__CT_ENTRY, entry->value->datasize, entry->value->data, entry->key, 0, 0, NULL);
    
    //envia message para servidor e recebe resposta
    struct message_t *msg_resposta = network_send_receive(rtable, msg);
    if (msg_resposta == NULL) {
        destroy_mensage_t_sent(msg);
        return -1;
    }

    destroy_mensage_t_sent(msg);
    destroy_mensage_t_received(msg_resposta);

    return 0;

}

/* Função para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key) {

    if (rtable == NULL || key == NULL) return NULL;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_GET, MESSAGE_T__C_TYPE__CT_KEY, 0, NULL, key, 0, 0, NULL);

    //envia message para servidor e recebe resposta
    struct message_t *msg_resposta = network_send_receive(rtable, msg);
    if (msg_resposta == NULL) {
        destroy_mensage_t_sent(msg);
        return NULL;
    }

    char* data_copy = msg_resposta->msg->data.data != NULL ? strdup((char*)msg_resposta->msg->data.data) : NULL;
    struct data_t *data = data_create2(msg_resposta->msg->data.len, data_copy);

    destroy_mensage_t_sent(msg);
    destroy_mensage_t_received(msg_resposta);

    return data;
}

/* Função para remover um elemento da tabela. Vai libertar 
 * toda a memoria alocada na respetiva operação rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key) {

    if (rtable == NULL || key == NULL) return -1;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_DEL, MESSAGE_T__C_TYPE__CT_KEY, 0, NULL, key, 0, 0, NULL);

    //envia message para servidor e recebe resposta
    struct message_t *msg_resposta = network_send_receive(rtable, msg);
    if (msg_resposta == NULL) {
        destroy_mensage_t_sent(msg);
        return -1;
    }

    if(msg_resposta->msg->opcode == MESSAGE_T__OPCODE__OP_ERROR) {
        destroy_mensage_t_sent(msg);
        destroy_mensage_t_received(msg_resposta);
        return -1;
    }

    destroy_mensage_t_sent(msg);
    destroy_mensage_t_received(msg_resposta);

    return 0;
}

/* Devolve o número de elementos contidos na tabela.
 */
int rtable_size(struct rtable_t *rtable) {

    if (rtable == NULL) return -1;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_SIZE, MESSAGE_T__C_TYPE__CT_NONE, 0, NULL, NULL, 0, 0, NULL);

    //envia message para servidor e recebe resposta
    struct message_t *msg_resposta = network_send_receive(rtable, msg);
    if (msg_resposta == NULL) {
        destroy_mensage_t_sent(msg);
        return -1;
    }

    destroy_mensage_t_sent(msg);

    int size = msg_resposta->msg->result;

    destroy_mensage_t_received(msg_resposta);

    return size;
}

/* Devolve um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable) {

    if (rtable == NULL) return NULL;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_GETKEYS, MESSAGE_T__C_TYPE__CT_NONE, 0, NULL, NULL, 0, 0, NULL);

    //envia message para servidor e recebe resposta
    struct message_t *msg_resposta = network_send_receive(rtable, msg);
    if (msg_resposta == NULL) {
        destroy_mensage_t_sent(msg);
        return NULL;
    }

    destroy_mensage_t_sent(msg);

    // numero de keys
    int size = msg_resposta->msg->n_keys;

    char **keys = (char **) malloc(sizeof(char*) * (size+1));
    if (keys == NULL) {
        perror("ERRO no keys malloc");
        return NULL; 
    }
    
    // copiar keys
    for (int i = 0; i < size; i++) {
        keys[i] = strdup(msg_resposta->msg->keys[i]);
    }
    keys[size] = NULL; // colocar ultimo elemento a null

    destroy_mensage_t_received(msg_resposta);

    return keys;
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys){
    if (keys != NULL) {
        for (int i = 0; keys[i] != NULL; i++) {
            free(keys[i]);
        }
        free(keys);
    }
}

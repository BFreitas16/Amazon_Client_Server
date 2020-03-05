/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

#include"table_skel.h"
#include "message-private.h"

struct task_t {
    int op_n; //o número da operação
    int op; //a operação a executar. op=0 se for um delete, op=1 se for um put
    char* key; //a chave a remover ou adicionar
    char * data; // os dados a adicionar em caso de put, ou NULL em caso de delete
    struct task_t * next; //adicionar campo(s) necessário(s) para implementar fila do tipo produtor/consumidor
};

/*
* Adiciona uma task à queue e atualiza last_assigned.
* Retorna o numero da task se esta foi adicionada ou -1 no caso de ERRO.
*/
int add_task_to_queue(int op, char* key, struct data_t * data);

/*
* Retira uma task à queue e atualiza op_count.
* Retorna uma task se esta foi retirada ou NULL no caso de ERRO.
*/
struct task_t * take_task_from_queue();

/*
 * Handler da operacao size
 */
void handler_op_size(MessageT * mensagem, struct message_t *msg);

/*
 * Handler da operacao del
 */
void handler_op_del(MessageT * mensagem, struct message_t *msg);

/*
 * Handler da operacao get
 */
void handler_op_get(MessageT * mensagem, struct message_t *msg);

/*
 * Handler da operacao put
 */
void handler_op_put(MessageT * mensagem, struct message_t *msg);

/*
 * Handler da operacao getkeys
 */
void handler_op_getkeys(MessageT * mensagem, struct message_t *msg);

/*
* Handler da operacao verify
*/
void handler_op_verify(MessageT * mensagem, struct message_t *msg);
/*
* Preenche a mensagem quando ocorre um bad request
*/
void bad_request (struct message_t *msg);

/*
* Preenche a mensagem quando ocorre um erro na operação a ser executada na tabela
*/
void op_error (struct message_t *msg, int error_code);

#endif
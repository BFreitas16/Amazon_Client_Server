/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>


#include "table_skel-private.h"
#include "table.h"
#include "table-private.h"
#include "message-private.h"

struct table_t *table_amazon;

/*
 * HANDLERS DE OPERAÇOES
 * Nota: Estes tratam dos pedidos e produzem as respeticas mensagens
 */

/*
 * Handler da operacao size
 */
void handler_op_size(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_NONE)
        mensagem->result = table_size(table_amazon);
    else
        bad_request(msg);
}

/*
 * Handler da operacao del
 */
void handler_op_del(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_KEY) {
        if (table_del(table_amazon, mensagem->key) == 0)
            mensagem->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        else
            op_error(msg, -1);
    } else {
        bad_request(msg);
    }
}

/*
 * Handler da operacao get
 */
void handler_op_get(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_KEY) {
        struct data_t *data = table_get(table_amazon, mensagem->key);

        if (data != NULL) {
            mensagem->c_type = MESSAGE_T__C_TYPE__CT_VALUE;

            mensagem->data.len = data->datasize;

            // alocar para copiar a data
            mensagem->data.data = malloc(data->datasize);
            if (mensagem->data.data == NULL) {
                perror("ERRO no malloc do data ");
                op_error(msg, 0);
            } else {
                // copiar a data
                memcpy(mensagem->data.data, data->data, data->datasize);
                data_destroy(data);
            }
        } else {
            op_error(msg, 0);
        }
    } else {
        bad_request(msg);
    }
}

/*
 * Handler da operacao put
 */
void handler_op_put(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_ENTRY) {
        struct data_t *temp_data = data_create2(mensagem->data.len,mensagem->data.data);

        if (table_put(table_amazon, mensagem->key, temp_data) == 0)
            mensagem->c_type = MESSAGE_T__C_TYPE__CT_NONE;  
        else
            op_error(msg, -1);

        free(temp_data);
    } else {
        bad_request(msg);
    }
}

/*
 * Handler da operacao getkeys
 */
void handler_op_getkeys(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_NONE) {

        mensagem->c_type = MESSAGE_T__C_TYPE__CT_KEYS;

        // numero de keys
        mensagem->n_keys = table_size(table_amazon);

        char ** keys = table_get_keys(table_amazon);
        if (keys == NULL) {
            op_error(msg, 0);
        }

        // allocar para copiar as keys
        mensagem->keys = malloc(sizeof(char *) * (table_amazon->size));
        for (int i = 0; i < table_amazon->size; i++) {
            // copiar cada key
            mensagem->keys[i] = strdup(keys[i]);
        }

        table_free_keys(keys);
    } else {
        bad_request(msg);
    }
}

// Funcoes da interface

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists){
    
    if ((table_amazon = table_create(n_lists)) == NULL) {
        perror("ERRO no table_create");
        return -1;
    }
    
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){
    table_destroy(table_amazon);
}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct message_t *msg){

    if (table_amazon == NULL || msg == NULL) return -1;
    
    MessageT *mensagem = msg->msg;

    switch (mensagem->opcode)
    {
    case MESSAGE_T__OPCODE__OP_SIZE:
        handler_op_size(mensagem, msg);
        break;
    case MESSAGE_T__OPCODE__OP_DEL:
        handler_op_del(mensagem, msg);
        break;
    case MESSAGE_T__OPCODE__OP_GET:
        handler_op_get(mensagem, msg);
        break;
    case MESSAGE_T__OPCODE__OP_PUT:
        handler_op_put(mensagem, msg);
        break;
    case MESSAGE_T__OPCODE__OP_GETKEYS:
        handler_op_getkeys(mensagem, msg);
        break;
    default:
        mensagem->opcode = MESSAGE_T__OPCODE__OP_BAD;
        mensagem->c_type = MESSAGE_T__C_TYPE__CT_BAD;
    }
    
    // se sucesso opcode = opcode + 1
    if (mensagem->opcode != MESSAGE_T__OPCODE__OP_ERROR && mensagem->opcode != MESSAGE_T__OPCODE__OP_BAD) 
        mensagem->opcode += 1;

    return 0;
}

// Funcoes de tratamento de erros

void bad_request (struct message_t *msg) {
    MessageT *mensagem = msg->msg;
    
    mensagem->opcode = MESSAGE_T__OPCODE__OP_ERROR;
    mensagem->c_type = MESSAGE_T__C_TYPE__CT_BAD;
}

void op_error (struct message_t *msg, int error_code) {
    MessageT *mensagem = msg->msg;

    mensagem->opcode = MESSAGE_T__OPCODE__OP_ERROR;

    if (error_code == 0)
        mensagem->c_type =  MESSAGE_T__C_TYPE__CT_NONE;
    else {
        mensagem->c_type =  MESSAGE_T__C_TYPE__CT_RESULT;
        mensagem->result = error_code;
    }
    
}

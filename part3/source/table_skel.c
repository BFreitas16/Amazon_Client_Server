/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>
#include <pthread.h>

#include "table_skel-private.h"
#include "table.h"
#include "table-private.h"
#include "message-private.h"

struct table_t *table_amazon;
int last_assigned = 0;
int op_count = 0;
struct task_t *queue_head = NULL;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t table_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

pthread_t thread_process_task;

int thread_em_execucao = 1;

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

    /* criação de nova thread */
	if (pthread_create(&thread_process_task, NULL, &process_task, NULL) != 0){
		perror("ERRO Thread não foi criada no skel_init");
        return -1;
	}

    return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){
    
    thread_em_execucao = 0;
    pthread_cond_signal(&queue_not_empty);

    if (pthread_join(thread_process_task, NULL) != 0) {
        perror("ERRO ao fazer join da thread.");
    }

    table_destroy(table_amazon);
    pthread_mutex_destroy(&queue_lock);
    pthread_mutex_destroy(&table_lock);
    pthread_cond_destroy(&queue_not_empty);

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
    case MESSAGE_T__OPCODE__OP_VERIFY:
        handler_op_verify(mensagem, msg);
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

/* Verifica se a operação identificada por op_n foi executada.
*/
int verify(int op_n) {
    return op_n < op_count;
}

/* Função do thread secundário que vai processar pedidos de escrita.
* Falta fzr op_count++
*/
void * process_task (void *params) {
    while(thread_em_execucao) {
        struct task_t *task = take_task_from_queue();
        if (task != NULL) {
            pthread_mutex_lock(&table_lock);
            switch(task->op) {
                case 0: // DELETE
                    table_del(table_amazon, task->key);
                    break;
                case 1: // PUT
                    table_put(table_amazon, task->key, (struct data_t *) task->data);
                    data_destroy((struct data_t *) task->data);
                    break;
                default:
                    break;
            }
            pthread_mutex_unlock(&table_lock);
            op_count++;
            
            free(task->key);
            free(task);
        }
    }
    return NULL;
}

/*
* Adiciona uma task à queue e atualiza last_assigned.
* Retorna o numero da task se esta foi adicionada ou -1 no caso de ERRO.
*/
int add_task_to_queue(int op, char* key, struct data_t * data) {
    if (op < 0 || key == NULL) return -1;

    int op_n = last_assigned;
    last_assigned++;

    struct task_t * new_task = (struct task_t *) malloc(sizeof(struct task_t));
    if (new_task == NULL) {
        perror("ERRO no malloc de new_task");
        return -1;
    }
    new_task->op_n = op_n;
    new_task->op = op;
    new_task->key = key;
    new_task->data = (char *) data;
    new_task->next = NULL;

    pthread_mutex_lock(&queue_lock);
    if (queue_head == NULL) {
        queue_head = new_task;

        pthread_cond_signal(&queue_not_empty); /* Avisar que queue_head != NULL */
    }
    else {
        struct task_t * current = queue_head;

        // current fica com o ultimo elemento da queue
        while (current->next != NULL)
            current = current->next;

        current->next = new_task;
    }
    pthread_mutex_unlock(&queue_lock);

    return op_n;
}

/*
* Retira uma task à queue.
* Retorna uma task se esta foi retirada ou NULL no caso de ERRO.
*/
struct task_t * take_task_from_queue() {

    struct task_t * removed_task = NULL;


    pthread_mutex_lock(&queue_lock);

    /* Esperar por queue_lock > 0 */
    while (queue_head == NULL && thread_em_execucao)
        pthread_cond_wait(&queue_not_empty, &queue_lock);

    if (thread_em_execucao) {
        removed_task = queue_head;
        queue_head = queue_head->next;
    }
    
    pthread_mutex_unlock(&queue_lock);


    return removed_task;
}

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
        int res = add_task_to_queue(0,strdup(mensagem->key),NULL);
        if (res != -1) {
            mensagem->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            mensagem->result = res;
        } else
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
        struct data_t *temp_data = data_create(mensagem->data.len);
        memcpy(temp_data->data,mensagem->data.data,temp_data->datasize);
    
        int res = add_task_to_queue(1, strdup(mensagem->key), temp_data);
        if (res != -1) {
            mensagem->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            mensagem->result = res;  
        } else
            op_error(msg, -1);


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

void handler_op_verify(MessageT * mensagem, struct message_t *msg) {
    if (mensagem->c_type == MESSAGE_T__C_TYPE__CT_RESULT) {
        int op_n = mensagem->result;
        int was_executed = verify(op_n);
        if(was_executed >= 0) {
            
            mensagem->result = was_executed;
        }
        else
            op_error(msg, -1);
    } else {
        bad_request(msg);
    }
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
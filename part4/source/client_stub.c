/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#include"client_stub-private.h"
#include "network_client.h"

static server_t *head = NULL;
static server_t *tail = NULL;

#define TIMEOUT 2000

static char *root_path = "/chain";
static zhandle_t *zh;
static int is_connected;
typedef struct String_vector zoo_string;
static char *watcher_ctx = "ZooKeeper Childreen Watcher";

////////////////////////////// A  ////////////////////////////////////////

/**
* Data Watcher function for /MyData node
*/
static void child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx);

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
    if(rtable == NULL)
        return 0;
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

    int op_n = msg_resposta->msg->result;

    destroy_mensage_t_received(msg_resposta);

    return op_n;

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

    int op_n = msg_resposta->msg->result;

    destroy_mensage_t_received(msg_resposta);

    return op_n;
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

/* Verifica se a operação identificada por op_n foi executada.
    // Pedro pensa que
    // -1 Erro
    // > 0 Executou
    // 0 Não executou a operacao
*/
int rtable_verify(struct rtable_t *rtable, int op_n) {

    if (rtable == NULL || op_n < 0) return -1;

    //preenche struct message_t
    struct message_t *msg = make_message(MESSAGE_T__OPCODE__OP_VERIFY, MESSAGE_T__C_TYPE__CT_RESULT, 0, NULL, NULL, op_n, 0, NULL);

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

    int was_executed = msg_resposta->msg->result;

    destroy_mensage_t_sent(msg);
    destroy_mensage_t_received(msg_resposta);

    return was_executed;
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

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna -1 em caso de erro.
 */
int zookeeper_connect(const char *address_port) {
    
    zh = zookeeper_init(address_port, connection_watcher, TIMEOUT, 0, 0, 0);
    if (zh == NULL) {
		perror("Error connecting to ZooKeeper server!\n");
		return -1;
	}

    sleep(3);
    if (is_connected) {
   
        zoo_string * children_list = (zoo_string *) malloc(sizeof(zoo_string));
        char * zdata_buf = (char *)malloc(ZDATALEN * sizeof(char));
        if (ZNONODE == zoo_exists(zh, root_path, 0, NULL)) {
            perror("Error: There are no servers!");
            free(zdata_buf);
            return -1;
        }
        if (ZOK != zoo_wget_children(zh, root_path, &child_watcher, watcher_ctx, children_list)) {
            fprintf(stderr, "Error setting watch at %s!\n", root_path);
            free(zdata_buf);
            return -1;
        }

        if(updateHeadAndTail(children_list) == -1) {
            free(zdata_buf);
            free(children_list);
            return -1;
        }

        free(zdata_buf);
        free(children_list);
	} else {
        perror("ERRO ao ligar ao zookeeper");
        return -1;
    }
    return 0;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int zookeeper_disconnect() {
    int res = rtable_disconnect(head);

    // se o outro não tiver fechado com sucesso
    // devolve -1 mesmo se este fechar bem
    if(rtable_disconnect(tail) == -1) {
        res = -1;
    }

    zookeeper_close(zh);

    return res;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int zookeeper_put(struct entry_t *entry) {
    return rtable_put(head, entry);
}

/* Função para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *zookeeper_get(char *key) {
    return rtable_get(tail, key);
}

/* Função para remover um elemento da tabela. Vai libertar 
 * toda a memoria alocada na respetiva operação rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int zookeeper_del(char *key) {
    return rtable_del(head, key);
}

/* Devolve o número de elementos contidos na tabela.
 */
int zookeeper_size() {
    return rtable_size(tail);
}

/* Devolve um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento a NULL.
 */
char **zookeeper_get_keys() {
    return rtable_get_keys(tail);
}

/* Verifica se a operação identificada por op_n foi executada.
*/
int zookeeper_verify(int op_n) {
    return rtable_verify(tail, op_n);
}

///////////////////////////////// FUNÇÕES AUXILIARES /////////////////////////////
/**
* Watcher function for connection state change events
*/
void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void* context) {
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			is_connected = 1; 
		} else {
			is_connected = 0; 
		}
	} 
}

/*
 * Deolve o id do node
 */
int getIdfromNode(char * node) {
    char *token = strtok(node, "node");
    return atoi(token);
}

int findMinMaxNode(char ** min, char ** max, zoo_string* children_list) {

    if (children_list == NULL || children_list->count == 0) return -1;

    char * minimum_node = children_list->data[0];
    int minimum_node_id = getIdfromNode(minimum_node);
    char * maximum_node = minimum_node;
    int maximum_node_id = minimum_node_id;

    char * current_node = NULL;
    int id = -1;
    for (int i = 1; i < children_list->count; i++)  {
        current_node = children_list->data[i];
        id = getIdfromNode(current_node);
        
        if(id < minimum_node_id) {
            minimum_node = current_node;
            minimum_node_id = id;
        }

        if(id > maximum_node_id) {
            maximum_node = current_node;
            maximum_node_id = id;
        }
    }
    *min = minimum_node;
    *max = maximum_node;

    return 0;
}

char * getNodeData(char * node_name) {
    if (node_name == NULL) return NULL;

    char node_path[120] = "";
    strcat(node_path, root_path); 
    strcat(node_path,"/"); 
    strcat(node_path, node_name);

    char *node_data = malloc(ZDATALEN * sizeof(char));
    int node_data_len = ZDATALEN;

    zoo_get(zh, node_path, 0, node_data, &node_data_len, NULL);

    return node_data;
}
/*
Atualiza o head e tail recebidos na children list
caso tenham mudado deve ligar se aos novos servidores
*/
int updateHeadAndTail(zoo_string* children_list) {

    char * head_node;
    char * tail_node;

    if (findMinMaxNode(&head_node, &tail_node, children_list) == -1) {
        rtable_disconnect(head);
        head = NULL;
        rtable_disconnect(tail);
        tail = NULL;
        printf("Não há servidores disponíveis neste momento...\n");
        return -1;
    }

    char * head_address_port = getNodeData(head_node);
    printf("HEAD ADRESS: %s\n", head_address_port);
    char * tail_address_port = getNodeData(tail_node);
    printf("TAIL ADRESS: %s\n", tail_address_port);

    // se o head tiver alterado crio uma nova conexão
    if(head == NULL || strcmp(head->hostname, head_address_port) != 0) {
        rtable_disconnect(head);
        if ((head = rtable_connect(head_address_port)) == NULL)
            return -1;
    }

    // se o tail tiver alterado crio uma nova conexão
    if(tail == NULL || strcmp(tail->hostname, tail_address_port) != 0) {
        rtable_disconnect(tail);
        if ((tail = rtable_connect(tail_address_port)) == NULL)
            return -1;
    }

    free(head_address_port);
    free(tail_address_port);
    
    return 0;
}
/**
* Data Watcher function for /MyData node
*/
static void child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx) {
    
    zoo_string* children_list =	(zoo_string *) malloc(sizeof(zoo_string));

	if (state == ZOO_CONNECTED_STATE) {
		if (type == ZOO_CHILD_EVENT) {
	 	   /* Get the updated children and reset the watch */ 
 			if (ZOK != zoo_wget_children(zh, root_path, child_watcher, watcher_ctx, children_list)) {
 				fprintf(stderr, "Error setting watch at %s!\n", root_path);
 			}

            updateHeadAndTail(children_list);
		 } 
	 }
	 free(children_list);
}


/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<signal.h> 

#include"client_stub.h"
#include"client_stub-private.h"

#define MAX             100
#define QUIT            "quit"
#define PUT_KEY_DATA    "put"
#define GET_KEY         "get"
#define DEL_KEY         "del"
#define SIZE            "size"
#define GETKEYS         "getkeys"
#define SEPARATOR       " "

struct rtable_t *table;

/*  
 *  Funcao que verifica se address_port contem dois campos: o hostname e a port
 *  NOTA: funcao nao verifica se o hostname ou o porto estao corretos apenas se os contem
 *   123.123.123:34 Aceita
 *   0:0:0          Não aceita
 *   0              Não aceita
 *   0:a            Aceita
 */
int is_address_port_valid(char * address_port) {
    char * copy_address_port = strdup(address_port);
    int count = 0;
    char *token = strtok(copy_address_port, ":");
    do {
        int length = strlen(token);
        if(length == 0) return 0;
        count++;
    } while ( (token = strtok(NULL, ":")) != NULL );
    free(copy_address_port);
    return count == 2 ? 1 : 0;
}

/*
 * Disconnect do client
 */
void close_client()
{
	printf ("O cliente vai terminar agora\n");
    rtable_disconnect(table);
	exit (0);
}

/* 
 * HANDLERS DE OPERACOES
 * Nota: Estes tratam dos pedidos e imprimem as respeticas mensagens de resposta
 */

/*
 * Handler da operacao put
 */
void handler_put_key_value() {
    char *key = strtok(NULL, SEPARATOR);

    // tudo a seguir ao espaço que depois de key
    // fizemos assim porque o data pode conter espaços
    char *data_read = strtok(NULL, "");

    if (data_read != NULL) {
        // criar data
        struct data_t *data = data_create2(strlen(data_read)+1, strdup(data_read));
        
        // criar entry
        struct entry_t *entry = entry_create(strdup(key), data);

        if (rtable_put(table, entry) != -1)
             printf("PUT realizado com sucesso\n");
        else
            printf("ERRO ao realizar a operação PUT\n");

        entry_destroy(entry);
    } else {
        printf("ERRO ao realizar a operação PUT (poucos argumentos: put <key> <data>)\n");
    }
}

/*
 * Handler da operacao get
 */
void handler_get_key() {
    char *key = strtok(NULL, SEPARATOR);

    struct data_t *data = rtable_get(table, key);

    if(data != NULL) {
        if (data->datasize == 0 || data->data == NULL)
            printf("Key não existente na tabela\n");
        else 
            printf("VALUE: %s\n",(char*) data->data);
    } else {
        printf("ERRO ao realizar a operação GET\n");
    }

    data_destroy(data);
}

/*
 * Handler da operacao del
 */
void handler_del_key() {
    char *key = strtok(NULL, SEPARATOR);

    if (rtable_del(table, key) != -1)
        printf("DEL realizado com sucesso\n");
    else
        printf("ERRO ao realizar a operação DEL (Key pode nao existir)\n");
}

/*
 * Handler da operacao size
 */
void handler_size() {
    int size = rtable_size(table);
    if (size != -1)
        printf("SIZE: %d\n",size);
    else
        printf("ERRO ao realizar a operação SIZE\n");
}

/*
 * Handler da operacao getkeys
 */
void handler_getkeys() {
    char **keys = rtable_get_keys(table);
    if(keys != NULL) {
        printf("KEYS:\n");

        int i;
        for(i = 0; keys[i] != NULL; i++) {
            printf("Key %d: %s\n", i+1, keys[i]);
        }
        
        // se i=0 nao entrou no loop, logo nao ha keys
        if (i == 0) printf("Nao existem keys na tabela\n");
    } else {
        printf("ERRO ao realizar a operação GETKEYS\n");
    }
    rtable_free_keys(keys);
}

// MAIN
int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    if(argc < 2) {
        printf("ERRO: Exemplo de uso \'table-client <hostname>:<port>\'\n");
        exit(0);
    }

    if(!is_address_port_valid(argv[1])) {
        printf("ERRO: Fornecer ip e porto da seguinte forma: \'<hostname>:<port>\'\n");
        exit(0);
    }

    table = rtable_connect(argv[1]);
    if (table == NULL) {
        return -1;
    }

    // Instruções de comandos possiveis para comunicar com o servidor
    printf("Ligacão establecida a %s com sucesso.\nComandos possiveis:\n", table->hostname);
    printf("\tput <key> <data>\n");
    printf("\tget <key>\n");
    printf("\tdel <key>\n");
    printf("\tsize\n");
    printf("\tgetkeys\n");
    printf("\tquit\n");

    char buf[MAX];
    char *token;
    while(fgets(buf, MAX, stdin)){

        //remove a mudança de linha
        token = strtok(buf, "\n");
        
        token = strtok(token, SEPARATOR);

        if (strcmp(token, QUIT) == 0) {
            close_client();
            return 0;
        }

        if (strcmp(token, PUT_KEY_DATA) == 0) { // PUT
            handler_put_key_value();
        }
        else if (strcmp(token, GET_KEY) == 0) { // GET
            handler_get_key();
        }
        else if (strcmp(token, DEL_KEY) == 0) { // DEL
            handler_del_key();
        }
        else if (strcmp(token, SIZE) == 0) { // SIZE
            handler_size();
        }
        else if (strcmp(token, GETKEYS) == 0) { // GETKEYS
            handler_getkeys();
        }
        else {
            printf("ERRO: \'%s\' não é um comando reconhecido\n", token);
        }
    }
    return 0; 
}

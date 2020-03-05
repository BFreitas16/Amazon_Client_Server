/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#include "network_client.h"
#include "client_stub-private.h"
#include "sdmessage.pb-c.h"

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable) {

    if (rtable == NULL) return -1;

    int sockfd;

    // Criar socket TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERRO ao criar socket TCP");
        return -1;
    }

    struct sockaddr_in *server = rtable->server_addr;

    // Preencher estrutura server para estabelecer conexao
    server->sin_family = AF_INET;
    server->sin_port = htons(rtable->port); // Porta TCP
    
    if (inet_pton(AF_INET, rtable->hostname, &server->sin_addr) < 1)
    {
        perror("ERRO ao converter IP");
        return -1;
    }

    // Estabelecer conexão com o servidor
    if (connect(sockfd, (struct sockaddr *) rtable->server_addr, sizeof(*(rtable->server_addr))) < 0)
    {
        perror("ERRO ao conectar-se ao servidor");
        close(sockfd);
        return -1;
    }

    rtable->sockfd = sockfd;

    return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtable_t * rtable, 
                                        struct message_t *msg){

    if (rtable == NULL || msg == NULL) return NULL;

    int sockfd = rtable->sockfd;

    // Enviar a mensagem
    if (write_all(msg, sockfd) == -1) {
        close(sockfd);
        return NULL;
    }

    struct message_t *message;

    // Receber a mensagem
    if ((message = read_all(sockfd)) == NULL) {
        close(sockfd);
        return NULL;
    }

    return message;
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtable_t * rtable) {
    if (rtable == NULL) return -1;
    close(rtable->sockfd);
    return 0;
}
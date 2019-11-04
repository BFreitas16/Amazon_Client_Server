/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include"network_server.h"
#include"message-private.h"

// GLOBAL variables
struct sockaddr_in server; // estrutura server com endereço(s) para associar (bind) à socket 
int sockfd; // socket
int connsockfd; // cliente (só existe 1 cliente)

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port) {
    
    // Criar socket TCP
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERRO ao criar socket TCP");
        return -1;
    }

    // Preencher estrutura server para estabelecer conexao
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // Porta TCP
    server.sin_addr.s_addr = htonl(INADDR_ANY); // Todos os endereços na máquina
    
    // Aproveitar porto usado anteriormente no bind
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(sockfd);
        return -1;
    }

    // Fazer bind 
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("ERRO ao fazer bind");
        close(sockfd);
        return -1;
    }

    // Fazer listen
    if (listen(sockfd, 0) < 0)
    {
        perror("ERRO ao executar listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket) {

    struct sockaddr_in client;
    socklen_t size_client = 0;

    // primeira conexao
    if ((connsockfd = accept(listening_socket,(struct sockaddr *) &client, &size_client)) == -1) {
        perror("ERRO ao conectar-se ao cliente");
        close(connsockfd);
        return -1;
    }

    // Informar que um cliente está conectado
    char ip_buff[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &client.sin_addr, ip_buff,size_client) != NULL)
        printf("Cliente %s conectou-se com sucesso.\n", ip_buff);

    while (1)
    {
        // receber msg
        struct message_t * msg;
        if ( (msg = network_receive(connsockfd) ) == NULL) { // Permitir que o cliente se desconecte e volte a conectar
            if (inet_ntop(AF_INET, &client.sin_addr, ip_buff,size_client) != NULL)
                printf("Cliente %s desconectou-se com sucesso.\n", ip_buff);
            
            close(connsockfd);

            // Informar que um cliente se desconectou
            connsockfd = accept(listening_socket,(struct sockaddr *) &client, &size_client);
            if (inet_ntop(AF_INET, &client.sin_addr, ip_buff,size_client) != NULL)
                printf("Cliente %s conectou-se com sucesso.\n", ip_buff);
            continue;
        }
        
        // entregar ao skeleton e esperar
        if (invoke(msg) == -1) {
            perror("ERRO ao invocar a função");
            close(connsockfd);
            return -1;
        }

        // enviar msg
        if (network_send(connsockfd, msg) == -1) {
            close(connsockfd);
            return -1;
        }

        // destroy_mensage_t_sent(msg);
        destroy_mensage_t_received(msg);
    }

    return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket) {
    return read_all(client_socket);
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg) {
    return write_all(msg, client_socket);
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close() {
    close(sockfd);
    return 0;
}
/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include <errno.h>
#include <poll.h>
// #include <fcntl.h>

#include"network_server.h"
#include"message-private.h"

// GLOBAL variables
struct pollfd connections[NFDESC]; // Estrutura para file descriptors das sockets das ligações
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

    for (int i = 0; i < NFDESC; i++)
        connections[i].fd = -1;    // poll ignora estruturas com fd < 0

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

    int nfds, kfds;
    char ip_buff[INET_ADDRSTRLEN]; // para informar quando cliente se conecta ou desconecta

    struct sockaddr_in client;
    socklen_t size_client = 0;

    connections[0].fd = sockfd;  // Vamos detetar eventos na welcoming socket
    connections[0].events = POLLIN;  // Vamos esperar liga��es nesta socket

    nfds = 1; // número de file descriptors

// Retorna assim que exista um evento ou que TIMEOUT expire. * FUN��O POLL *.
    while ((kfds = poll(connections, nfds, 10)) >= 0) { // kfds == 0 significa timeout sem eventos
    
        if (kfds > 0){ // kfds � o n�mero de descritores com evento ou erro

            if ((connections[0].revents & POLLIN) && (nfds < NFDESC))  // Pedido na listening socket ?
                if ((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *) &client, &size_client)) > 0){ // Liga��o feita ?
                    connections[nfds].events = POLLIN; // Vamos esperar dados nesta socket
                    nfds++;
                    
                        // Informa qual o cliente que se conectou
                    if (inet_ntop(AF_INET, &client.sin_addr, ip_buff,size_client) != NULL)
                        printf("Cliente %s conectou-se com sucesso.\n", ip_buff);
                }
            for (int i = 1; i < nfds; i++) { // Todas as liga��es

            if (connections[i].revents & POLLIN) { // Dados para ler ?

                // receber msg
                struct message_t * msg;
                if ( (msg = network_receive(connections[i].fd) ) == NULL) { // Permitir que o cliente se desconecte e volte a conectar
                    if (inet_ntop(AF_INET, &client.sin_addr, ip_buff,size_client) != NULL)
                        printf("Cliente %s desconectou-se com sucesso.\n", ip_buff);
                    
                    close(connections[i].fd);
                    connections[i].fd = -1;
                    continue;
                } else {
                    // entregar ao skeleton e esperar
                    if (invoke(msg) == -1) {
                        perror("ERRO ao invocar a função");
                        close(connections[i].fd);
                        connections[i].fd = -1;
                        continue;
                    }

                    // enviar msg
                    if (network_send(connections[i].fd, msg) == -1) {
                        close(connections[i].fd);
                        connections[i].fd = -1;
                        continue;
                    }

                    // destroy_mensage_t_sent(msg);
                    destroy_mensage_t_received(msg);
                }
            }

            if((connections[i].revents & POLLERR) || (connections[i].revents & POLLHUP)) {
                close(connections[i].fd);
                connections[i].fd = -1;
            }
            }

        }
    }

    // Fecha socket de listening (s� executado em caso de erro...)
    close(sockfd);

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
/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include <signal.h>

#include "network_server.h"
#include "table_skel.h"

/* 
 * Signal handler
 */
void signal_handler(int signo)
{
    table_skel_destroy();
    network_server_close(); 
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    
    /* Testar os argumentos de entrada */
    if(argc != 3) {
        printf("Error: Example of use \'table-server <port> <n_lists>\'\n");
        exit(0);
    }

    short port = atoi(argv[1]);
    int n_lists = atoi(argv[2]);
    if(port == 0 || n_lists == 0) {
        printf("Error: Example of use \'table-server <port> <n_lists>\'\n <port> and <n_lists> must be numbers\n");
        exit(0);
    }

    // inicialização da camada de rede
    int socket_de_escuta = network_server_init(port);
    if (socket_de_escuta == -1) {
        perror("ERRO ao preparar socket para conexão");
        exit(-1);
    }

    // inicialização do skell
    if (table_skel_init(n_lists) == -1) {
        perror("ERRO ao criar a tabela");
        table_skel_destroy();
        network_server_close();    
        exit(-1);
    }
    
    // aceitar, processar e responder a pedidos de clientes
    int result = network_main_loop(socket_de_escuta);
    if (result == -1) {
        perror("ERRO na conexão do cliente");
        table_skel_destroy();
        network_server_close();    
        exit(-1);
    } 

    // Fechar
    table_skel_destroy();
    network_server_close();
    
    return 0;

}


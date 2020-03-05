/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<signal.h>
#include <netdb.h> 

// #include <net/if.h>
// #include <sys/ioctl.h>

#include "network_server.h"
#include "table_skel.h"

char * getIP(int fd) {
    char hostbuffer[256]; 
    char *IPbuffer; 
    struct hostent *host_entry;
    int hostname; 

    hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    if (hostname == -1) 
    { 
        perror("gethostname"); 
        return NULL; 
    } 

    // To retrieve host information 
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) 
    { 
        perror("gethostbyname"); 
        return NULL;
    } 
    
    // To convert an Internet network 
    // address into ASCII string 
    IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); 
    return IPbuffer;
}
/* 
 * Signal handler
 */
void signal_handler(int signo)
{
    table_skel_destroy();
    network_server_close(); 
    printf("network closed\n");
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    
    /* Testar os argumentos de entrada */
    if(argc != 4) {
        printf("ERRO: Exemplo de uso \'table-server <porto> <n_listas> <IP>:<porta>\'\n");
        printf("Nota: O primeiro argumento <porto> refere-se ao porto da máquina enquanto que o par <IP>:<porta> é referente ao ZoeKeepeer.\n");
        exit(0);
    }

    short port = atoi(argv[1]);
    int n_lists = atoi(argv[2]);
    if(port == 0 || n_lists == 0) {
        printf("Error: Example of use \'table-server <port> <n_lists>\'\n <port> and <n_lists> must be numbers\n");
        exit(0);
    }

    char * zookeeper_host_port = argv[3];

    // inicialização da camada de rede
    int socket_de_escuta = network_server_init(port);
    if (socket_de_escuta == -1) {
        perror("ERRO ao preparar socket para conexão");
        exit(-1);
    }

    // struct sockaddr_in server;
    // char ip_buff[INET_ADDRSTRLEN];

    // server.sin_family = AF_INET;
    // server.sin_port = htons(port);
    // server.sin_addr.s_addr = 

    // este if era o nosso ahah
    // if (inet_ntop(AF_INET, &server.sin_addr, ip_buff,INET_ADDRSTRLEN) != NULL)
    //     printf("Este é o servidor: %s\n", ip_buff);   <- ESTE MÉTODO NÃO FUNCIONA PQ O IP É 0? What do u mean? 
    
    
    //IMPRIMIR IP AQUI A PARTIR DE socket_de_escuta (ver função do discord)
    char * ip = getIP(socket_de_escuta);
    if (ip == NULL) {
        network_server_close();
        perror("ERRO ao obter IP");    
        exit(-1);
    }

    strcat(ip,":");
    strcat(ip,argv[1]);

    printf("Este é o servidor: %s\n", ip);
    printf("-------------------------------------------\n");

    // inicialização do skell
    if (table_skel_init(n_lists, zookeeper_host_port, ip) == -1) {
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


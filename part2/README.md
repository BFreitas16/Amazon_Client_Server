# Projeto 2

* Bruno Freitas
* Marcelo Mouta
* Pedro Marques

Neste ficheiro vamos incluir o modo de utilização do ficheiro *makefile* e alguns detalhes de implementação em relação a cada ficheiro do projeto bem como alguns dos problemas/dificuldades que surgiram durante a realização dos mesmos.

De notar que tanto para o cliente como para o servidor foram executados testes feitos pelo grupo, em conjunto com a ferramenta *valgrind*, para verificar quaisquer erros ou leaks na memória.

----------------------------------
## Make
  Foi feito um make para simplificar o processo de compilar e executar algumas funçoes. As funções dadas pelo make são:
  * ```make``` : Compila todos os ficheiros, deixando os executáveis dos ficheiros de teste na diretoria *grupo03/binary* e as bibiliotecas em *grupo03/lib*
  * ```make client-lib.o``` : Compila todos os ficheiros referentes ao cliente, à excecão do ficheiro *table-client* que contem o *main()*, criando a biblioteca *grupo03/lib/client-lib.o*
  * ```make table-client``` : Compila todos os ficheiros referentes ao cliente, criando o executável *grupo03/binary/table-client*
  * ```make table-server``` : Compila todos os ficheiros referentes ao servidor, criando o executável *grupo03/binary/table-server*
  * ```make clean``` : elimina todos os binarios e ficheiros objetos contidos em *grupo03/binary*, *grupo03/object* e *grupo03/lib* respetivamente.

----------------------------------

## Parte Projeto 1
  
  #### Alterações:
  * Foram alterados os ficheiros table.c e data.c de modo a que, na função table_get, quando uma chave não é encontrada seja devolvida uma estrutura data_t com size=0 e data=NULL.
  * Devido ao facto do cliente poder pedir para consultar as keys, com uso do getkeys, protegemos o método `table_free_keys` do table.c para não tentar libertar espaço de keys a `NULL`.

----------------------------------

## Parte Projeto 2

  #### Restrições
  * Só é permitido troca de mensagens entre servidor e cliente (e vice versa) de no máximo de 2048 bytes.
    
    Nota: No início tinhamos feito um modelo que enviava primeiro uma mensagem com um inteiro (o tamanho do que iria enviar) e depois era enviado a mensagem "real", o que se queria enviar. Este método foi abandonado por questões de simplicidade.

  * Para ler comandos na aplicação interativa *table-client* espera-se que cada comando não contenha mais do que 100 caracteres.

## Protocolo Buffer
A estrutura *message* presente no ficheiro *.proto* foi alterada de modo a melhor permitir a representação de todo o tipo de mensagens enviadas na rede, resultando na seguinte estrutura:
```
message message_t
 {
        enum Opcode {
                OP_BAD     = 0;
                OP_SIZE    = 10;
                OP_DEL     = 20;
                OP_GET     = 30;
                OP_PUT     = 40;
                OP_GETKEYS = 50;
                OP_ERROR   = 99;
        }
        Opcode opcode = 1;

        enum C_type {
                CT_BAD    = 0;
                CT_KEY    = 10;
                CT_VALUE  = 20;
                CT_ENTRY  = 30;
                CT_KEYS   = 40;
                CT_RESULT = 50;
                CT_NONE   = 60;
        }
        C_type c_type = 2;

        string key = 3;

        bytes data = 5;

        sint32 result = 6;
        
        repeated string keys = 7;
};
```
Em que os parametros adicionados são utilizados para as seguintes funções:
* ```string key``` : representa a key de uma entrada na tabela, utilizada nas operações *put*, *get* e *del*
* ```bytes data``` : representa a data de uma entrada na tabela, utilizada nas operações *put* e *get*
* ```sint32 result``` : representa o resultado da operação *size* OU código de erro de uma dada operação
* ```repeated string keys``` : representa as keys presentes na tabela, utilizada na operação *getkeys*

Apesar de alguns dos parametros da *message_t* não serem sempre utilizados em todas as operações, foi mantida apenas uma destas estruturas ao invés de uma específica para cada operação em questão. Esta decisão deve-se ao facto de se querer evitar uma maior complexidade na leitura das mesmas mensagens, que a nosso ver, seria desnecessária.

## Network
* Ligações TCP com o protocolo IPv4.

Seguimos o conselho dos professores e criamos um ficheiro *message.c* no qual foram desenvolvidas as funções extras `read_all`e `write_all` para receber e escrever respetivamente mensagens, assim como a estrutura de dados *message_t*. Foi criado uma função `make_message` para simplificar o processo de criação de uma mensagem *message_t*: 

*message-private.h*

```c
// Estrutura mensagem
struct message_t
{
    MessageT *msg;
};

/*
 * Serializa mensagem e envia para a rede
 * Retorna 0 (OK) ou -1 (erro)
 */
int write_all(struct message_t * msg_to_send, int sockfd);

/*
 * Lê e deserializa a mensagem a ser recebida pela rede
 * Retorna a  mensagem recebida ou NULL em caso de erro.
 */
struct message_t * read_all(int sockfd);

/*
 * Cria uma message passando-lhe os parametros que são relevantes
 */
struct message_t * make_message(MessageT__Opcode opcode, MessageT__CType ctype, int data_size, void *data, char * key, int32_t result, size_t n_keys, char **keys);
```


##### Dificuldades:
* Os métodos `network_send_receive` do `network_client` e o `network_receive` do `network_server` tivemos complicações no que toca ao envio e receção de mensagens por escolhas de implementação complexas que acabamos por alterar.

  
#### network client
* Nada a acrescentar.

#### network server
* Foram introduzidos prints para informar quando um dado cliente se conecta/desconecta ao servidor. Os mesmos informam também qual o endereço do cliente em questão, recorrendo à função *inet_ntop* para o fazer. 

----------------------------------

## Skel e Stub

Estruturas de dados desenvolvidas: 

*client_stub-private.h*
```c
struct rtable_t {
    char *hostname;
    unsigned short port;
    struct sockaddr_in *server_addr;
    int sockfd;
};
```
Aqui, os parâmetros *hostname* e *port* guardam o endereço e a porta do servidor ao qual o cliente se pretende ligar. A estrutura *server_addr* guarda as informações referentes ao endereço do socket pelo qual a ligação vai ser establecida e o *sockfd* guarda um descritor de ficheiro referente a esse mesmo socket.

  #### client stub
  * Nada a acrescentar.
  
  #### server skel
  * Para simplificar o código decidimos criar procedimentos auxiliares, Handlers, como o `handler_op_put`, para abstrair do invoke as funções relativas às operações na tabela.

----------------------------------

## table-(client e server)

  #### table-client
  * Para simplificar o código decidimos criar procedimentos auxiliares, Handlers, como o `handler_put_key_value`, para abstrair do main as funções relativas com a *rtable*.
  * O client não fecha mesmo que o servidor vá abaixo. Isto foi feito propositadamente, pois por exemplo, o facebook quando vai abaixo não manda as aplicações clientes abaixo, apenas não se conseguem conectar.

  #### table-server
  * Foram feitos os tratamentos para os *signals* *SIGPIPE*, *SIGINT* e *SIGQUIT*. O primeiro foi ignorado de acordo com a observação dos professores, enquanto que os seguintes foram tratados em `signal_handler` de modo a que o servidor termine aquando da receção de um CTRL+C ou CTRL+\\.

----------------------------------

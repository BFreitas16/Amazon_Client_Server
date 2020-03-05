# Projeto 3
## Grupo 03
* Bruno Freitas 51468
* Marcelo Mouta 51110
* Pedro Marques 51021

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

## Parte Projeto 2
  
  #### Alterações:
  * Foram alterados o ficheiro *table-client.c* para incluir a nova operação `verify`.
  * Na mensagem do *protobuf*, foi adicionado um novo elemento ao enumerado `Opcode`: `OP_VERIFY`.

----------------------------------

## Parte Projeto 3

  #### Restrições
  * O servidor pode servir no máximo 50 clientes concorrentes.

## Protocolo Buffer
Na mensagem, foi adicionado um novo elemento ao enumerado `Opcode`:
`OP_VERIFY  = 60;`

## Network

#### Network Server
* Adicionou-se uma constante `TIMEOUT` com o tempo máximo de espera para 50 milisegundos.
* Adicionou-se uma constante `NFDESC` com o numero 50 de sockets (onde uma é para ficar listening)
* Alterou-se o método `network_server_init` para permitir servir multiplos clientes ao invés de apenas um como tinhamos feito na última fase
* Consequentemente o método `network_main_loop` tambem foi alterado para, então, poder permitir. De notar que foram adicionadas verificações para os casos do `network_send` e `network_receive` porque como está referenciado mais à frente os métodos `write_all` e `read_all` do message.c podem retornar valores de erro caso os parametros sejam errados, por exemplo

----------------------------------

## Skel e Stub

  #### Client stub
  * Adicionou-se a função `int rtable_verify(struct rtable_t *rtable, int op_n)` que cria uma mensagem nova para ser enviada ao `network_client`. Esta mensagem contêm no campo result o `op_code` da operação a ser verificada se foi executada ou não no lado do servidor devolvendo `1` e `0` respetivamente.
  * Alterou-se o método `rtable_put` e `rtable_del` para permitir devolver o respetivo código da operação.
  
  #### Server skel
  * Alterou-se o método `int invoke(struct message_t *msg)` para incluir a nova operação `verify`.
  * Adicionou-se a `table_skel-private.h` a estrutura:
```c
struct task_t {
  int op_n; //o número da operação
  int op; //a operação a executar. op=0 se for um delete, op=1 se for um put
  char* key; //a chave a remover ou adicionar
  char* data; // os dados a adicionar em caso de put, ou NULL em caso de delete
  struct task_t * next;
}
```
  * De modo a implementar uma queue adicionou-se a variável global `task_t *queue_head` que aponta para o inicio da queue.
  * Adicionou-se a função `int add_task_to_queue(int op, char* key, char* data)` para adicionar operações/tarefas à queue.
  * Adicionou-se as variáveis globais `pthread_mutex_t queue_lock`, `pthread_mutex_t table_lock`para controlar o acesso concorrente à queue e tabble respetivamente. E adicionou-se `pthread_cond_t queue_not_empty` para notificar quando a queue deixa de estar vazia.
  * Nota: Inicialmente o grupo tinha pensado ter uma variavel `op_count` que iria sendo incrementada na função `take_task_from_queue` para contar as operações. Posteriormente chegou-se à conclusão que essa variável só deveria ser incrementada quando fosse fazer a operação, logo passou para o método `xx`
  * Adicionou-se um método `verify` em que consite em avalizar a condição `op_n < op_count` em que `op_n` é a mensagem que o numero da operação que se quer verificar e `op_count` é o código da última operação executada.
  * Foi adicionado no `table_skel_init` a `criação da thread_process_task` que fica à esperade ter uma task para fazer. Foi tambem, consequentemente, adicionado o detach da thread no método `table_skel_destroy`.
  * Foi adicionado o metodo `process_task` para processar a task de fazer delete ou fazer um put
  
  ##### Problemas:
  * Tivemos problemas no `table_skel_destroy` pois primeiramente tinhamos o detach mas dava erro ao fazer, depois do detach, o destroy do mutex e da condição. Então posteriormente mudámos para `pthread_cancel` mas isto tinha um problema pois haviam leaks visto que a thread construia uma estrutura e não a libertava. 
  * Atualmente implementámos a `table_skel_destroy` com uma variavel global `thread_em_execucao` para indicar quando a thread deve acabar, libertando assim todos os recursos. Nesta versão, utilizamos também o *signal * da condição `queue_not_empty` de modo a que o *wait* desta condição não fique eternamente à espera, na função `take_task_from_queue`, que é executada pela thread na função `process_task`.
  
----------------------------------

## table-(client e server)

  #### table-client
  * Adicionou-se a operação `verify` ao cliente. Esta recebe um `op_code` e devolve uma mensagem a indicar se essa operação já foi executada ou não.
  * Adicionou-se o método `handler_verify` que verifica se uma operação foi executada.
  * Alterou-se o texto que o terminal devolve quando é executado a operação `put` e `del`para retornar ao cliente o número da operação correspondente à operação que estão a tentar executar.

  #### table-server
  * Adicionou-se o método `void * process_task (void *params)` que é executado pela thread que é criada para executar as operações sobre a tabela retirando tarefas da queue.

----------------------------------

## Message
* Aterou-se os métodos write_all e read_all adicionando à sua assinatura um novo parâmetro `int sockfd` e a verificando se este é válido para continuar a execução do método normalmente.

----------------------------------

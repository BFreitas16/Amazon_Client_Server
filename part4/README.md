# Amazon_Client_Server

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

## Parte Projeto 4

  #### Restrições
  * Um servidor pode servir no máximo 50 clientes concorrentes.
  * Imaginando que temos 3 servidores ligados ao zookeeper. O cliente, ao fazer uma operação de put, irá introduzir uma entry na tabela e os 3 servidores ligados irão ter o mesmo conteúdo e o `op_count` (conta o numero de operações feitas) irá ser igual nos dois. Se for adicionado um quarto servidor, esse irá ser o tail e acontece que irá dizer que não tem a entry que foi adicionada inicialmente, nem irá ter o `op_count` igual. Se voltarmos a adicionar a mesma entry, a operação irá propagar-se por todos os servidores ficando todos com a mesma entry (os 3 primeiros atualizam o seu conteudo e o último adiciona) mas o `op_count` não irá ser igual na mesma (os 3 primeiros servidores irão ter um `op_count` maior)

  #### Implementação dos servidores:
  * Os servidores ligam-se uns aos outros (Ligação em LISTA ligada), de forma a que, quando o head recebe uma instrução, propaga-a por todos os servidores até ao tail para ter-se consistencia dos dados e ter tolerância a faltas. Assim o grupo optou por dizer que um servidor A que se está a conectar a um servidor B é um cliente de B (utilizando assim a interface de cliente).

## Skel e Stub

  #### Client stub
  * De modo a implementar a chain adicionou-se as variáveis globais `server_t *head`
`server_t *tail` que aponta para o servidor head e tail respetivamente.
  * Foram adicionados novos métodos para lidar com as situações do zookeeper e uns adicionais:
  ```c
  /**
  * Watcher function for connection state change events
  */
  void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void* context);

  /*
  * Deolve o id do node
  */
  int getIdfromNode(char * node);

  void findMinMaxNode(char ** min, char ** max, zoo_string* children_list);

  char * getNodeData(char * node_name);

  /*
  * Atualiza o head e tail recebidos na children list
  * caso tenham mudado deve ligar se aos novos servidores
  */
  void updateHeadAndTail(zoo_string* children_list);

  /**
  * Data Watcher function for /MyData node
  */
  static void child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx);

  int table_skel_init(int n_lists, char* zookeeper_host_port);

  /// ZOOKEEPER

  /* Função para estabelecer uma associação entre o cliente e o servidor, 
  * em que address_port é uma string no formato <hostname>:<port>.
  * Retorna -1 em caso de erro.
  */
  int zookeeper_connect(const char *address_port);

  /* Termina a associação entre o cliente e o servidor, fechando a 
  * ligação com o servidor e libertando toda a memória local.
  * Retorna 0 se tudo correr bem e -1 em caso de erro.
  */
  int zookeeper_disconnect();

  /* Função para adicionar um elemento na tabela.
  * Se a key já existe, vai substituir essa entrada pelos novos dados.
  * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
  */
  int zookeeper_put(struct entry_t *entry);

  /* Função para obter um elemento da tabela.
  * Em caso de erro, devolve NULL.
  */
  struct data_t *zookeeper_get(char *key);

  /* Função para remover um elemento da tabela. Vai libertar 
  * toda a memoria alocada na respetiva operação rtable_put().
  * Devolve: 0 (ok), -1 (key not found ou problemas).
  */
  int zookeeper_del(char *key);

  /* Devolve o número de elementos contidos na tabela.
  */
  int zookeeper_size();

  /* Devolve um array de char* com a cópia de todas as keys da tabela,
  * colocando um último elemento a NULL.
  */
  char **zookeeper_get_keys();

  /* Verifica se a operação identificada por op_n foi executada.
  */
  int zookeeper_verify(int op_n);
  ```
  * Foi definido um valor máximo de Znode Data máximo suportado: 1MB
  
  #### Server skel
  * Foram adicionados parametros no table_skel_init: `int table_skel_init(int n_lists, char* zookeeper_host_port, char *my_ip)`
  * Foram adicionadas novas funções para lidar com novas situações
  ```c
  #define TIMEOUT 2000 

  /*
  * Watcher function for connection state change events
  */
  void connect_watcher(zhandle_t *zzh, int type, int state, const char *path, void* context);

  /**
  * Data Watcher function for /Chain node
  */
  static void chain_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx);

  /*
  * Devolve o node imediatamente a seguir a este, ou NULL caso não haja
  */
  char * current_next_node(zoo_string *children_list);

  /*
  * O next server alterou-se?
  */
  int next_server_changed(char *new_next_node);

  /*
  * Connects to the next server, given its node
  */
  int next_server_connect(char *next_node);

  /**
  * Deolve o id do node a partir do seu path
  */
  int getIdfromPath(char * path);
  ```
  * O `process_task` foi alterado de forma a que as operações de escrita, o `put` e o `del`, sejam feitas localmente e depois propagadas para o próximo servidor (bastou utilizar a interface do cliente mencionada acima)

A estrutura rtable_t é agora partilhada pelo skel e o stub (Ver secção `Implementação dos servidores`):  
```c
struct rtable_t {
    char *hostname;
    unsigned short port;
    struct sockaddr_in *server_addr;
    int sockfd;
    char * znode; // node deste server
};

typedef struct rtable_t server_t;
```
  
----------------------------------

## table-(client e server)

Ambos foram alterados para lidar com o zookeeper

  #### table-client
  * Tivemos de ter o cuidado de fazer `zookeeper_disconnect()` ao desligar o cliente

  #### table-server
  * Foi alterado o table_server para receber 3 argumentos: <porto> <n_listas> <IP>:<porta>
  * Adicionou-se um metodo `char * getIP(int fd)` para ir buscar o ip

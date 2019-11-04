# Projeto 1 
## Grupo 03
* Bruno Freitas 51478 
* Marcelo Mouta 51110 
* Pedro Marques 51021

Neste ficheiro vamos incluir o modo de utilização do ficheiro *makefile* e alguns detalhes de implementação em relação a cada ficheiro do projeto bem como alguns dos problemas/dificuldades que surgiram durante a realização dos mesmos.

De notar que para cada ficheiro presente foram executados testes, em conjunto com a ferramenta *valgrind*, para verificar quaisquer erros ou leaks na memória.

----------------------------------
## Make
  Foi feito um make para simplificar o processo de compilar e executar algumas funçoes. As funções dadas pelo make são:
  * ```make``` : Compila todos os ficheiros, deixando os executáveis dos ficheiros de teste na diretoria *grupo03/binary*
  * ```make test``` : corre todos os testes dados pelos professores
  * ```make valgrind ``` : corre o valgrind associado a um teste dado pela variável **TEST**. *Exemplo de utilização: ```make valgrind TEST="test_data"```*
  * ```make clean``` : elimina todos os binarios e ficheiros objetos contidos em *grupo03/binary* e *grupo03/object* respetivamente.

----------------------------------

## Data
  
  #### Problemas/dificuldades: 
  * Foi feito a classe *data.c* e quando vizualizamos o *test_data.c* reparamos que tinhamonos esquecido de condições para salvaguardar, ou seja, não faziamos verificações se os paramoetros de entrada eram válidos

----------------------------------

## Entry
  
  #### Problemas/dificuldades:
  * Foi semelhante ao data, não foram encontradas muitas dificuldades, já que a sua concretização é semelhante à do *data.c*

----------------------------------

## List

Estruturas de dados desenvolvidas: *list-private.h*
```c
struct node_t {
    struct entry_t *entry;
    struct node_t *next;
};

struct list_t {
    int size; /* numero de elementos (entry) da lista */
    struct node_t *head; /* primeiro elemento da lista */
    struct node_t *tail; /* ultimo elemento da lista */
}; 
```

  #### Problemas/dificuldades:
  * Esqueciamo-nos de remover referencias (colocar a *NULL*) - Ex: no *list_destroy*
  * No *list_free_keys* tinhamo-nos esquecido de dar *free* das *keys*
  * No *list_get_keys*, numa primeira fase fizemos uma implementação em que se fosse dada uma lista vazia iria ser retornada uma lista com um elemento NULL. FOI MELHORADO, posteiormente,  para uma implementação mais eficiente onde retorna apenas *NULL*
  * A maior dificuldade encontrou-se no *list_remove()* pois tivemos que desenvolver o algoritmo de raiz. Inclusive foram feitas várias versões. Os problemas advinham do facto de nós não estarmos a pensar em todas possibilidades com que o algoritmo se poderia deparar. Ex: remover um nó entre 2 nós.
        
----------------------------------

## Table

Estruturas de dados desenvolvidas: *table-private.h*
```c
struct table_t {
    int size;   /* numero de elementos da tabela */
    int n;      /* numero de entradas da tabela */
    struct list_t **lists;
};
```

Metodos adicionais/auxiliares
```c
/* Funcao que faz o hash de uma string,
* onde key é a key
* onde n representa o tamanho do array
*/
int hash(char *key, int n);

/* Funcao que faz a base do hash -> hashCode da string */
int hashCode(char *key);

/* Funcao que junta a uma lista keys, partir do index
* para a frente, uma lista de strings keys_to_add de tamanho size
*/
void concat_keys(char **keys_to_add, int size, char **keys, int index);
```
  #### Limitação
  * A table foi feita com um numero fixo de posições que não muda independentemente de estar sobrelotada ou não. Note-se que se for criada uma table com 10 posições esta não irá no futuro ser expandida mesmo que `LOAD_FACTOR` da tabela esteja bastante acima do aconselhado para manter uma complexidade _O(1)_ amortizado nas suas operações. Por outras palavras, se a table tiver MUITOS elementos, isto tem prejuizos diretos na efiência.
  
  #### Problemas/dificuldades encontradas:
  * No *table_put*, numa primeira fase, tinhamos implementado com a ajuda dos metodos *list_remove(key)* de seguida de um *list_add(elemento)*. FOI MELHORADA, posteriormente, para uma implementação onde, após verificarmos que uma key já existia na tabela, utilizámos o o apontador para o *value* correspondente, de modo a apontar para o novo *value*, fazendo também o *free* necessário para o que lá se encontrava antes.
  * *table_get_keys* foi necessario uma função auxiliar *concat_keys* para ajudar no processo de contrução. Nesta função tinhamos de ter o cuidado de fazer o free do "bloco/caixa de fora", ou seja, da *keys_to_add*
  * No *table_get_keys*, tal como no *list_get_keys* FOI MELHORADO, posteiormente,  para uma implementação mais eficiente onde retorna apenas *NULL* como consequencia foi colocado uma verificação no inicio do metodo *concat_keys*

----------------------------------

## Serialization

  **Nota:** No inicio foram feitas verificações para prevenir o erro de não chegarem os dados todos: *(pseudo-código)* 
  ```c 
  If ( buff_size < total_length ) return ERRO
  ``` 
  No final chegamos por não incluir pois iria tornar o código muito menos legível e foi falado com o professor, que nos disse que não era necessário no ambito desta fase/projeto

  #### Problemas/dificuldades encontradas
  * Numa primeira implementação, a função *data_to_buffer* não funcionava pois utilizámos mal o apontador duplo ***data_buf*. Ao invés de usarmos acertadamente **data_to_buffer* para reservar memoria e colocar os dados necessários, usavamos diretamente *data_to_buffer*.

----------------------------------

## Testes Complementares


/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "table-private.h"
#include "list-private.h"

/* Função para criar/inicializar uma nova tabela hash, com n
* linhas (n = módulo da função hash)
* Em caso de erro retorna NULL.
*/
struct table_t *table_create(int n)
{
    if (n < 1) return NULL;

    // Reservas
    struct list_t **ls;
    ls = (struct list_t **) malloc(n * sizeof(struct list_t *));    
    if (ls == NULL) {
        perror("ERRO no list malloc do table_create");
        return NULL;
    }

    struct table_t *t;
    t = (struct table_t *) malloc(sizeof(struct table_t));
    if (t == NULL) {
        perror("ERRO no table malloc do table_create");
        return NULL;
    }

    // Inicializar listas
    for (int i = 0; i < n; i++)
    {
        ls[i] = list_create();
    }

    // Iniciar tabela
    t->size = 0;
    t->n = n;
    t->lists = ls;

    return t;
}

/* Função para libertar toda a memória ocupada por uma tabela.
*/
void table_destroy(struct table_t *table)
{
    struct list_t **list = table->lists;

    for (int i = 0; i < table->n; i++)
    {
        list_destroy(list[i]);
    }

    free(list);

    free(table);
}

/* Função para adicionar um par chave-valor à tabela.
* Os dados de entrada desta função deverão ser copiados, ou seja, a
* função vai *COPIAR* a key (string) e os dados para um novo espaço de
* memória que tem de ser reservado. Se a key já existir na tabela,
* a função tem de substituir a entrada existente pela nova, fazendo
* a necessária gestão da memória para armazenar os novos dados.
* Retorna 0 (ok), em caso de adição ou substituição, ou -1 (erro), em
* caso de erro.
*/
int table_put(struct table_t *table, char *key, struct data_t *value)
{
    if (table == NULL || key == NULL || value == NULL) return -1;


    int h = hash(key, table->n);

    struct list_t *list = table->lists[h];

    // copiar a data e criar novo entry
    struct data_t *new_data = data_dup(value);

    // copiar a key
    char *new_key = strdup(key);
        
    struct entry_t *new_entry = entry_create(new_key, new_data);

    int previous_size = list->size;    
    if (list_add(list, new_entry) == -1) return -1;

    if (list->size > previous_size) table->size++;

    return 0;

}

/* Função para obter da tabela o valor associado à chave key.
* A função deve devolver uma cópia dos dados que terão de ser
* libertados no contexto da função que chamou table_get, ou seja, a
* função aloca memória para armazenar uma *CÓPIA* dos dados da tabela,
* retorna o endereço desta memória com a cópia dos dados, assumindo-se
* que esta memória será depois libertada pelo programa que chamou
* a função.
* Devolve NULL em caso de erro.
*/
struct data_t *table_get(struct table_t *table, char *key)
{
    if (table == NULL || key == NULL) return NULL;

    struct list_t *list = table->lists[hash(key, table->n)];

    struct entry_t *entry = list_get(list, key);

    if (entry != NULL)
    {
        return data_dup(entry->value);
    } else
    {
        // Caso em que a chave não é encontrada
        return data_create2(0,NULL);
    }
    

    return NULL;
}

/* Função para remover um elemento da tabela, indicado pela chave key,
* libertando toda a memória alocada na respetiva operação table_put.
* Retorna 0 (ok) ou -1 (key not found or error).
*/
int table_del(struct table_t *table, char *key)
{
    if (table == NULL || key == NULL) return -1;

    int h = hash(key, table->n);
    struct list_t *list = table->lists[h];

    if (list_get(list, key) == NULL) return -1;

    if (list_remove(list, key) == 0)
    {
        table->size--;
        
        return 0;
    }

    return -1;
}

/* Função que devolve o número de elementos contidos na tabela.
*/
int table_size(struct table_t *table)
{
    return table != NULL ? table->size : -1;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
* tabela, colocando o último elemento do array com o valor NULL e
* reservando toda a memória necessária.
*/
char **table_get_keys(struct table_t *table)
{
    if (table == NULL || table->size < 1) return NULL;

    char ** keys = (char **) malloc(sizeof(char *) * (table->size + 1));
    if (keys == NULL) {
        perror("ERRO no keys malloc do table_get_keys");
        return NULL;
    }

    // ultimo elemento é NULL
    keys[table->size] = NULL;

    int index = 0;

    for (int i = 0; i < table->n; i++)
    {
        struct list_t *list = table->lists[i];
        
        concat_keys(list_get_keys(list), list->size, keys, index); // nao libertamos a caixa grande

        index += list->size;
    }

    return keys;
}

/* Função que liberta toda a memória alocada por table_get_keys().
*/
void table_free_keys(char **keys)
{
    for (int i = 0; keys[i] != NULL; i++)
    {
        free(keys[i]);
    }

    free(keys);
}

/*
* Ira imprimir a tabela no formato:
*    Numero de posicoes da table = 2
*    Numero de entries da table = 4
*
*    Posicao 1:
*    Numero de elementos da lista = 4
*    [1, 2, 3, 4]
*
*    Posicao 2:
*    Numero de elementos da lista = 0
*    []
*/
void table_print(struct table_t *table)
{
    printf("Numero de posicoes da table = %d\n", table->n);
    printf("Numero de entries da table = %d\n\n", table->size);
    for (int i = 0; i < table->n; i++)
    {
        printf("Posicao %d:\n", i);
        list_print(table->lists[i]);
        printf("\n");
    }
}

/**#################**/
/**###  PRIVATE  ###**/
/**#################**/

/* Funcao que faz o hash de uma string, onde
* - key é a key
* - n representa o tamanho do array
*/
int hash(char *key, int n)
{
    return (hashCode(key) & 0x7fffffff) % n;
}

/* Funcao que faz a base do hash -> hashCode da string
*/
int hashCode(char *key)
{
    int hash = 7;
    
    for (int i = 0; i < strlen(key); i++)
    {
        hash = hash * 31 + key[i];
    }

    return hash;
}

/* Funcao que junta a uma lista keys, partir do index
* para a frente, uma lista de strings keys_to_add de tamanho size
*/
void concat_keys(char **keys_to_add, int size, char **keys, int index)
{
    if (keys_to_add == NULL) return;

    for (int i = 0; i < size; i++)
    {
        keys[index++] = keys_to_add[i];
    }

    // free do "bloco/caixa de fora"
    free(keys_to_add);
 
}
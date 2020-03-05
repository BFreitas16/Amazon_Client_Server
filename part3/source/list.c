/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "list-private.h"
#include "data.h"
#include "entry.h"

/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
* grupo no ficheiro list-private.h).
* Em caso de erro, retorna NULL.
*/
struct list_t *list_create() {

    struct list_t *list = (struct list_t *) malloc(sizeof(struct list_t));
    if (list == NULL) {
        perror("ERRO no list malloc do list_create");
        return NULL;
    } 

    list->head = NULL;
    list->tail = NULL;

    list->size = 0;
    
    return list;
}

/* Função que elimina uma lista, libertando *toda* a memoria utilizada
 * pela lista.
 */
void list_destroy(struct list_t *list)
{
    struct node_t * current = list->head;
    struct node_t * next;
  
    while (current != NULL)
    {
        next = current->next;

        entry_destroy(current->entry);
        free(current);

        current = next;
    }
    
    // remover referencias
    list->head = NULL;
    list->tail = NULL;

    // list->size = 0; 

    free(list);
}

/* Função que adiciona no final da lista (tail) a entry passada como
* argumento caso não exista na lista uma entry com key igual àquela
* que queremos inserir.
* Caso exista, os dados da entry (value) já existente na lista serão
* substituídos pelos os da nova entry.
* Retorna 0 (OK) ou -1 (erro).
*/
int list_add(struct list_t *list, struct entry_t *entry)
{
    if (entry == NULL || list == NULL) return -1;

    struct node_t *existing_node = list_get_node(list, entry->key);

    // entry com key igual à da entry dada já existe na lista?
    if (existing_node != NULL)
    {
        entry_destroy(existing_node->entry);

        existing_node->entry = entry;

        return 0;
    }

    // se a entry ainda nao existir

    struct node_t *node = (struct node_t *) malloc(sizeof(struct node_t));
    if (node == NULL) {
        perror("ERRO no node malloc do list_add");
        return -1;
    }

    node->entry = entry;
    node->next = NULL;

    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
    } else
    {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
    
    return 0;
}

/* Função que elimina da lista a entry com a chave key.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_remove(struct list_t *list, char *key)
{
    // o size == 0 eh erro
    if (key == NULL || list == NULL || list->size == 0) return -1;

    struct node_t *current_node = list->head;

    struct node_t *prev_node = NULL;

    // ainda nao removeu e nó corrente diff NULL
    while(current_node != NULL)
    {
        if (strcmp(current_node->entry->key, key) == 0)
        {
            // estamos no nó inicial?
            if (prev_node == NULL)
            {
                // [X] -> [ ] -> [ ]
                list->head = current_node->next;
            } else {
                // [ ] -> [X] -> [ ]
                prev_node->next = current_node->next;
            }

            // estamos no nó final?
            if (current_node->next == NULL)
            {
                // [ ] -> [ ] -> [X]
                list->tail = prev_node;
            }

            // remover a referencia            
            current_node->next = NULL;

            entry_destroy(current_node->entry);
            free(current_node);

            list->size--;

            return 0;
        }
        else
        {
            prev_node = current_node;
            current_node = current_node->next;
        }
    }

    return -1;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL em caso de erro.
 * Obs: as funções list_remove e list_destroy vão libertar a memória
 * ocupada pela entry ou lista, significando que é retornado NULL
 * quando é pretendido o acesso a uma entry inexistente.
*/
struct entry_t *list_get(struct list_t *list, char *key)
{
    if (list == NULL || key == NULL || list->size < 1) return NULL;

    struct node_t *node = list_get_node(list, key);
    if (node == NULL) return NULL;

    return node->entry;
}

/* Função que retorna o tamanho (número de elementos (entries)) da lista,
 * ou -1 (erro).
 */
int list_size(struct list_t *list)
{
    return list != NULL && list->size >= 0 ? list->size : -1;
}

/* Função que devolve um array de char* com a cópia de todas as keys da 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **list_get_keys(struct list_t *list)
{
    if(list == NULL || list->size < 1) return NULL; //  list->size < 1 ??

    char **keys = (char **) malloc((list->size + 1) * sizeof(char *));
    if (keys == NULL) {
        perror("ERRO no keys malloc do list_get_keys");
        return NULL;
    }

    // ultimo elemento a NULL
    keys[list->size] = NULL;

    struct node_t *current_node = list->head;

    for(int i = 0; i < list->size; i++) {
        keys[i] = (char *) malloc((strlen(current_node->entry->key) + 1) * sizeof(char));
        if (keys[i] == NULL) {
            perror("ERRO no key malloc do list_get_keys");
            return NULL;
        }

        strcpy(keys[i], current_node->entry->key);

        current_node = current_node->next;
    }
    
    return keys;
}

/* Função que liberta a memória ocupada pelo array das keys da tabela,
 * obtido pela função list_get_keys.
 */
void list_free_keys(char **keys)
{
    for (int i = 0; keys[i] != NULL; i++)
    {
        free(keys[i]);
    }

    free(keys);
}

/* Função que obtém da lista o node que contem a entry com a chave key.
 * Retorna a referência do node na lista ou NULL em caso de erro.
*/
struct node_t *list_get_node(struct list_t *list, char *key)
{
    if (list == NULL || key == NULL || list->size < 1) return NULL;

    struct node_t *current_node = list->head;

    while (current_node != NULL)
    {

        if (strcmp(current_node->entry->key, key) == 0)
        {
            return current_node;
        }

        current_node = current_node->next;
    }

    return NULL;
}

/*
* Ira imprimir a lista no formato:
*   Numero de elementos da lista = 4
*   [1, 2, 3, 4]
*/
void list_print(struct list_t *list)
{
    printf("Numero de elementos da lista: %d\n", list->size);

    struct node_t * current_node = list->head;

    printf("[");

    if (current_node != NULL)
    {
        entry_print(current_node->entry);
        current_node = current_node->next;
    }

    while (current_node != NULL)
    {
        printf(", ");
        entry_print(current_node->entry);
        current_node = current_node->next;
    }
    printf("]");

}
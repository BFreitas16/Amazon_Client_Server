/* Grupo 03
   Bruno Freitas 51478 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */
   
#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"
#include "entry.h"

struct node_t {
    struct entry_t *entry;
    struct node_t *next;
};

struct list_t {
    int size; /* numero de elementos (entry) da lista */
    struct node_t *head; /* primeiro elemento da lista */
    struct node_t *tail; /* ultimo elemento da lista */
};

/* Função que obtém da lista o node que contem a entry com a chave key.
 * Retorna a referência do node na lista ou NULL em caso de erro.
*/
struct node_t *list_get_node(struct list_t *list, char *key);

void list_print(struct list_t* list);

#endif

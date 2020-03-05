/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
    int size;   /* numero de elementos da tabela */
    int n;      /* numero de entradas da tabela */
    struct list_t **lists;
};

void table_print(struct table_t *table);

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

#endif

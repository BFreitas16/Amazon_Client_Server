/* Grupo 03
   Bruno Freitas 51468
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "entry.h"
#include "data.h"

/* Função que cria uma entry, reservando a memória necessária para a
* estrutura e inicializando os campos key e value, respetivamente, com a
* string e o bloco de dados passados como parâmetros, sem reservar
* memória para estes campos.
*/
struct entry_t *entry_create(char *key, struct data_t *data)
{
    if (key == NULL || data == NULL) return NULL;

    struct entry_t *e;
    e = (struct entry_t *) malloc(sizeof(struct entry_t));
    if (e == NULL) {
        perror("ERRO no entry malloc do entry_create");
        return NULL;
    }

    e->key = key;
    e->value = data;

    return e;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry)
{

    if (entry != NULL)
    {
        if (entry->key != NULL) free(entry->key);

        data_destroy(entry->value);

        free(entry);
    }
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry)
{
    if(entry == NULL) return NULL;
    
    struct entry_t *e;
    e = (struct entry_t *) malloc(sizeof(struct entry_t));
    if (e == NULL) {
        perror("ERRO no entry malloc do entry_dup");
        return NULL;
    }

    e->key = strdup(entry->key);
    e->value = data_dup(entry->value);

    return e;
}

/* Imprime a key da entry
*/
void entry_print(struct entry_t *entry)
{
    printf("Key: \"%s\"", entry->key);
}
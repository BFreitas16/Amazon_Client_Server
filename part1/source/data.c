/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 
*/

#include<stdlib.h>
#include<string.h>
#include <stdio.h>

#include "data.h"

/* Função que cria um novo elemento de dados data_t, reservando a memória
* necessária para armazenar os dados, especificada pelo parâmetro size
*/
struct data_t *data_create(int size)
{
   if (size < 1) return NULL;

   struct data_t *d;
   d = (struct data_t *) malloc(sizeof(struct data_t));
   if (d == NULL) {
        perror("ERRO no data malloc do data_create");
        return NULL;
   }

   d->datasize = size;
   d->data = malloc(size);

   return d;
}

/* Função que cria um novo elemento de dados data_t, inicializando o campo
* data com o valor passado no parâmetro data, sem necessidade de reservar
* memória para os dados.
*/
struct data_t *data_create2(int size, void *data)
{
   // Permite propositadamente que a data seja NULL
   if (size < 0) return NULL;

   struct data_t *d;
   d = (struct data_t *) malloc(sizeof(struct data_t));
   if (d == NULL) {
        perror("ERRO no data malloc do data_create2");
        return NULL;
   }

   d->datasize = size;
   d->data = data;

   return d;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data)
{

   if (data != NULL)
   {
      
      if (data->data != NULL) free(data->data);
      
      free(data);
   }
}

/* Função que duplica uma estrutura data_t, reservando toda a memória
* necessária para a nova estrutura, inclusivamente dados.
*/
struct data_t *data_dup(struct data_t *data)
{
   if (data == NULL || data->datasize < 1 || data->data == NULL) return NULL;

   struct data_t *d;
   d = (struct data_t *) malloc(sizeof(struct data_t));
   if (d == NULL) {
        perror("ERRO no data malloc do data_dup");
        return NULL;
   }

   d->datasize = data->datasize;
   d->data = malloc(data->datasize);
   if (d->data == NULL) {
        perror("ERRO no data->data malloc do data_dup");
        return NULL;
   }

   memcpy(d->data, data->data, data->datasize);

   return d;
}

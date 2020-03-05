/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "data.h"
#include "entry.h"
#include "serialization.h"

/* Serializa uma estrutura data num buffer que será alocado
* dentro da função. Além disso, retorna o tamanho do buffer
* alocado ou -1 em caso de erro.
*/
int data_to_buffer(struct data_t *data, char **data_buf)
{   
    if (data == NULL || data_buf == NULL) return -1;

    // converter datasize
    u_int32_t datasize = htonl(data->datasize);

    // tamanho total do buffer
    int total_size = sizeof(datasize) + data->datasize;

    *data_buf = (char *) malloc(total_size);

    // copiar para o buffer o size da data->data
    memcpy(*data_buf, &datasize, sizeof(datasize));

    // copiar para o buffer o data->data
    memcpy(*data_buf + sizeof(datasize), data->data, data->datasize);

    return total_size;
}

/* De-serializa a mensagem contida em data_buf, com tamanho
* data_buf_size, colocando-a e retornando-a numa struct
* data_t, cujo espaco em memoria deve ser reservado.
* Devolve NULL em caso de erro.
*/
struct data_t *buffer_to_data(char *data_buf, int data_buf_size)
{
    if (data_buf == NULL || data_buf_size < 1) return NULL;

    u_int32_t *net_size = malloc(sizeof(u_int32_t));

    // retirar do buffer o size da data->data
    memcpy(net_size, data_buf, sizeof(u_int32_t));

    int size = ntohl(*net_size);
    void *data = malloc(size);

    // retirar do buffer a data->data
    memcpy(data, data_buf + sizeof(u_int32_t), size);

    // criar a nova data
    struct data_t * new_data = data_create2(size, data);

    // free dos recursos que ja nao sao utilizados
    free(net_size);
    
    return new_data;
}

/* Serializa uma estrutura entry num buffer que sera alocado
* dentro da função. Além disso, retorna o tamanho deste
* buffer alocado ou -1 em caso de erro.
*/
int entry_to_buffer(struct entry_t *data, char **entry_buf)
{
    if (data == NULL || entry_buf == NULL) return -1;

    int size_string = strlen(data->key) + 1;
    int datasize = data->value->datasize;

    //tamanho total do buffer
    int total_size = sizeof(size_string) + size_string + sizeof(datasize) + datasize;

    *entry_buf = (char *) malloc(total_size);
    
    // Conversão de inteiros para formato network
    u_int32_t new_stringsize = htonl(size_string);
    u_int32_t new_datasize = htonl(datasize);
    
    // Copiar para buffer o tamanho da string - key 
    int offset = 0;
    memcpy(*entry_buf + offset, &new_stringsize, sizeof(new_stringsize));
    
    // Copiar para buffer a string - key 
    offset += sizeof(new_stringsize);
    memcpy(*entry_buf + offset, data->key, size_string);
    
    // Copiar para buffer o tamanho do value da entry
    offset += size_string;
    memcpy(*entry_buf + offset, &new_datasize, sizeof(new_datasize));
    
    // Copiar para buffer o value da entry
    offset += sizeof(new_datasize);
    memcpy(*entry_buf + offset, data->value->data, datasize);

    return total_size;
}

/* De-serializa a mensagem contida em entry_buf, com tamanho
* entry_buf_size, colocando-a e retornando-a numa struct
* entry_t, cujo espaco em memoria deve ser reservado.
* Devolve NULL em caso de erro.
*/
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size)
{
    if (entry_buf == NULL || entry_buf_size < 1) return NULL;

    u_int32_t *net_size_string = malloc(sizeof(u_int32_t));
    u_int32_t *net_size_data = malloc(sizeof(u_int32_t));

    // retirar do buffer o size da string - key
    int offset = 0;
    memcpy(net_size_string, entry_buf + offset, sizeof(u_int32_t));

    int size_string = ntohl(*net_size_string);

    char * key = malloc(size_string);

    // retirar do buffer a string - key
    offset += sizeof(u_int32_t);
    memcpy(key, entry_buf + offset, size_string);
    
    // retirar do buffer o size do value da entry
    offset += size_string;
    memcpy(net_size_data, entry_buf + offset, sizeof(u_int32_t));

    int size_data = ntohl(*net_size_data);

    void * data = malloc(size_data);

    // retirar do buffer o value da entry
    offset += sizeof(u_int32_t);
    memcpy(data, entry_buf + offset, size_data);
    
    // criar o novo entry
    struct data_t * new_data = data_create2(size_data, data);
    struct entry_t * new_entry = entry_create(key, new_data);
    
    // libertar os recursos que ja nao sao utilizados
    free(net_size_string);
    free(net_size_data);
    
    return new_entry;
}
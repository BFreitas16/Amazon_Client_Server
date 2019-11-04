/* Grupo 03
   Bruno Freitas 51468 
   Marcelo Mouta 51110 
   Pedro Marques 51021 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

#include"table_skel.h"
#include "message-private.h"

/*
* Preenche a mensagem quando ocorre um bad request
*/
void bad_request (struct message_t *msg);

/*
* Preenche a mensagem quando ocorre um erro na operação a ser executada na tabela
*/
void op_error (struct message_t *msg, int error_code);

#endif
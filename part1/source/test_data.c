#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "data.h"

void pee(const char *msg)
{
    perror(msg);
    exit(0);
}

/**************************************************************/
int testCreate() {
	int result;
	struct data_t *data;

	printf("Módulo data -> teste data_create:");

	assert(data_create(-1) == NULL);
	result = (data_create(-1) == NULL);

	assert(data_create(0) == NULL);
	result = result && (data_create(0) == NULL);

	if ((data = data_create(1024)) == NULL)
		pee("  O teste não pode prosseguir");

	memcpy(data->data,"1234567890a",strlen("1234567890a")+1);

	result = result &&
		 ((strcmp(data->data,"1234567890a") == 0) && (data->datasize == 1024));

	data_destroy(data);

	printf("data - testCreate: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
int testCreate2() {
	int result, data_size;
	struct data_t *data;
	//char *data_s = "1234567890abc";
	char *data_s = strdup("1234567890abc");
	data_size = strlen(data_s)+1;


	printf("Módulo data -> teste data_create2:");

	assert(data_create2(-1, data_s) == NULL);
	result = (data_create2(-1, data_s) == NULL);

    	assert(data_create2(0, data_s) == NULL);
	result = result && (data_create2(0, data_s) == NULL);

	assert(data_create2(data_size, NULL) == NULL);
	result = result && (data_create2(data_size, NULL) == NULL);

	if ((data = data_create2(data_size, data_s)) == NULL)
		pee("  O teste não pode prosseguir");

	result = result && (data->data == data_s);

	result = result &&
		 ((strcmp(data->data, data_s) == 0) && (data->datasize == data_size));

	data_destroy(data);

	printf("data - testCreate2: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDup() {
	//char *data_s = "1234567890abc";
	char *data_s = strdup("1234567890abc");
	int result, data_size = strlen(data_s)+1;
	struct data_t *data;
	struct data_t *data2;

	printf("Módulo data -> teste data_dup: ");

	assert(data_dup(NULL) == NULL);
	result = (data_dup(NULL) == NULL);

	if ((data = data_create(1)) == NULL)
		pee("  O teste não pode prosseguir");

	data->datasize = -1;

	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);

	free(data->data);
	
	data->data = NULL;
	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);
	
	data->datasize = 1;
	assert(data_dup(data) == NULL);
	result = result && (data_dup(data) == NULL);

	free(data);

	if ((data = data_create2(data_size,data_s)) == NULL)
		pee("  O teste não pode prosseguir");
	
	data2 = data_dup(data);
	
	result = result && (data2 != data);

	result = result && (data->data != data2->data) && 
                 (data->datasize == data2->datasize) &&
                 (memcmp(data->data, data2->data, data->datasize) == 0);

	data_destroy(data);
	data_destroy(data2);

	printf("data - testDup: %s\n",result?"passou":"não passou");
	return result;
}


/**************************************************************/
int testDestroy(){
    printf("Módulo data -> teste data_destroy:");
	data_destroy(NULL);
    printf(" passou\n");
	return 1;
}


/**************************************************************/
int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo data \n");

	score += testCreate();

	score += testCreate2();

	score += testDup();
	
	score += testDestroy();

	printf("teste data bin: %d/4\n",score);

    if (score == 4)
        return 0;
    else
        return -1;
}

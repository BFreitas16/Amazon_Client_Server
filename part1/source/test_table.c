#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "data.h"
#include "entry.h"
#include "table.h"

/**************************************************************/
int testTabelaVazia() {
	struct table_t *table = table_create(5);

	int result = table != NULL && table_size(table) == 0;

	table_destroy(table);
	
	printf("table - testTabelaVazia: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testPutInexistente() {
	int result, i;
	struct table_t *table = table_create(5);
	char *key[1024];
	struct data_t *data[1024], *d;

	for(i=0; i<1024; i++) {
		key[i] = (char*)malloc(16*sizeof(char));
		sprintf(key[i],"a/key/b-%d",i);
		data[i] = data_create2(strlen(key[i])+1,strdup(key[i]));

		table_put(table,key[i],data[i]);
	}

	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	for(i=0; i<1024; i++) {
		d = table_get(table,key[i]);

		assert(d->datasize == data[i]->datasize);
		assert(memcmp(d->data,data[i]->data,d->datasize) == 0);
		assert(d->data != data[i]->data);

		result = result && (d->datasize == data[i]->datasize && 
                           memcmp(d->data,data[i]->data,d->datasize) == 0 &&
                           d->data != data[i]->data);
		data_destroy(d);
	}

	for(i=0; i<1024; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}

	table_destroy(table);
	
	printf("table - testPutInexistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testPutExistente() {
	int result, i;
	struct table_t *table = table_create(5);
	char *key[1024];
	struct data_t *data[1024], *d;

	for(i=0; i<1024; i++) {
		key[i] = (char*)malloc(16*sizeof(char));
		sprintf(key[i],"a/key/b-%d",i);
		data[i] = data_create2(strlen(key[i])+1,strdup(key[i]));

		table_put(table,key[i],data[i]);
	}


	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	d = data_create2(strlen("256")+1,strdup("256"));
	table_put(table,key[256],d);
	data_destroy(d);

	assert(table_size(table) == 1024);
	result = result && (table_size(table) == 1024);

	for(i=0; i<1024; i++) {
		d = table_get(table,key[i]);
		
		if(i==256) {
			result = result && (d->datasize == strlen("256")+1 && 
        	                   memcmp(d->data,"256",d->datasize) == 0);
		} else {
			result = result && (d->datasize == data[i]->datasize && 
        	                   memcmp(d->data,data[i]->data,d->datasize) == 0 &&
        	                   d->data != data[i]->data);
		}

		data_destroy(d);
	}

	for(i=0; i<1024; i++) {
		free(key[i]);
		data_destroy(data[i]);
	}

	table_destroy(table);
	
	printf("table - testPutExistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDelInexistente() {
	int result, i;
	struct table_t *table = table_create(7);
	char *key;
	struct data_t *data;

	for(i=0; i<1024; i++) {
		key = (char*)malloc(16*sizeof(char));
		sprintf(key,"a/key/b-%d",i);
		data = data_create2(strlen(key)+1,key);

		table_put(table,key,data);

		data_destroy(data);
	}

	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	result = result && (table_get(table,"a/key/b-1024") == NULL) &&
			   (table_get(table,"abc") == NULL);

	result = result && (table_del(table,"a/key/b-1024") != 0) &&
			   (table_del(table,"abc") != 0);

	result = result && (table_get(table,"a/key/b-1024") == NULL) &&
			   (table_get(table,"abc") == NULL);

	assert(table_size(table) == 1024);
	result = result && (table_size(table) == 1024);

	table_destroy(table);
	
	printf("table - testDelInexistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDelExistente() {
	int result, i;
	struct table_t *table = table_create(7);
	char *key;
	struct data_t *data, *data2;

	for(i=0; i<1024; i++) {
		key = (char*)malloc(16*sizeof(char));
		sprintf(key,"a/key/b-%d",i);
		data = data_create2(strlen(key)+1,key);

		table_put(table,key,data);

		data_destroy(data);
	}

	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	result = result && ((data = table_get(table,"a/key/b-1023")) != NULL) &&
			   ((data2 = table_get(table,"a/key/b-45")) != NULL);

	data_destroy(data);
	data_destroy(data2);

	result = result && (table_del(table,"a/key/b-1023") == 0) &&
			   (table_del(table,"a/key/b-45") == 0);

	result = result && (table_get(table,"a/key/b-1023") == NULL) &&
			   (table_get(table,"a/key/b-45") == NULL);

	assert(table_size(table) == 1022);
	result = result && (table_size(table) == 1022);

	table_destroy(table);
	
	printf("table - testDelExistente: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testGetKeys() {
	int result = 1,i,j,achou;
	struct table_t *table = table_create(2);
	char **keys;
	char *k[4] = {"abc","bcd","cde","def"};
	struct data_t *d = data_create(5);

	table_put(table,k[3],d);
	table_put(table,k[2],d);
	table_put(table,k[1],d);
	table_put(table,k[0],d);

	data_destroy(d);

	keys = table_get_keys(table);
	
	for(i=0; keys[i] != NULL; i++) {
		achou = 0;
		for(j=0; j<4; j++) {
			achou = (achou || (strcmp(keys[i],k[j]) == 0));
		}
		result = (result && achou);
	}

	result = result && (table_size(table) == i);

	table_free_keys(keys);

	table_destroy(table);

	printf("table - testGetKeys: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int main() {
	int score = 0;

	printf("iniciando teste table bin\n");

	score += testTabelaVazia();

	score += testPutInexistente();

	score += testPutExistente();

	score += testDelInexistente();

	score += testDelExistente();

	score += testGetKeys();

	printf("teste table bin: %d/6\n",score);

    if (score == 6)
        return 0;
    else
        return -1;
}

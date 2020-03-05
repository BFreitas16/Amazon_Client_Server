#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "data.h"
#include "entry.h"
#include "list.h"

/**************************************************************/
int testListaVazia() {
	struct list_t *list = list_create();

	int result = list != NULL && list_size(list) == 0;

	list_destroy(list);
	
	printf("list - testListVazia: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testAddCauda() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *entry = entry_create(strdup("abc"),data_create(5));
	memcpy(entry->value->data,"abc1",5);

	list_add(list,entry);
	
	result = list_get(list,"abc") == entry && list_size(list) == 1;

	list_destroy(list);

	printf("list - testAddCauda: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testRemoveCabeca() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                    *e2 = entry_create(strdup("def"),data_create(5)),
                    *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list,"abc") == 0 && 
              list_get(list,"def") == e2 && 
              list_get(list,"ghi") == e3 && 
              list_size(list) == 2;

	list_destroy(list);

	printf("list - testRemoveCabeca: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testRemoveCauda() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                    *e2 = entry_create(strdup("def"),data_create(5)),
                    *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list,"ghi") == 0 && 
              list_get(list,"abc") == e1 && 
              list_get(list,"def") == e2 && 
              list_size(list) == 2;

	list_destroy(list);

	printf("list - testRemoveCauda: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testGetKeys() {
	int result;
	struct list_t *list = list_create();
	char **keys;
	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                    *e2 = entry_create(strdup("def"),data_create(5)),
                    *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	keys = list_get_keys(list);

printf("\n comp %d %s %s", strcmp(keys[0],e1->key), keys[0], e1->key);
printf("\n comp %d %s %s", strcmp(keys[1],e2->key), keys[1], e2->key);
printf("\n comp %d %s %s", strcmp(keys[2],e3->key), keys[2], e3->key);

	result = strcmp(keys[0],e1->key) == 0 && keys[0] != e1->key &&
              strcmp(keys[1],e2->key) == 0 && keys[1] != e2->key && 
              strcmp(keys[2],e3->key) == 0 && keys[2] != e3->key && 
              keys[3] == NULL;

	list_free_keys(keys);

	list_destroy(list);

	printf("\n list - testGetKeys: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/
/*Test By Miguel Guedelha*/

int testInsereMesmaKey() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
					*e2 = entry_create(strdup("def"),data_create(5)),
					*e3 = entry_create(strdup("abc"),data_create(5)),
					*e4 = entry_create(strdup("abc"),data_create(5)),
					*e5 = entry_create(strdup("ghi"),data_create(5)),
					*e6 = entry_create(strdup("def"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"abc2",5);
	memcpy(e4->value->data,"abc3",5);
	memcpy(e5->value->data,"ghi1",5);
	memcpy(e6->value->data,"def2",5);

	list_add(list,e1);
	list_add(list,e2);

	result = list_get(list, "abc") == e1 && list_get(list, "def") == e2;

	list_add(list,e3);

	result = result && list_get(list, "abc") == e3;

	list_add(list,e4);

	result = result && list_get(list, "abc") == e4;

	list_add(list,e5);
	list_add(list,e6);

	result = result && list_get(list, "ghi") == e5 && 
				list_get(list, "def") == e6;

	list_destroy(list);

	printf("list - testInsereMesmaKey: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/


int main() {
	int score = 0;

	printf("iniciando teste list bin\n");

	score += testListaVazia();

	score += testAddCauda();
	
	score += testInsereMesmaKey();

	score += testRemoveCabeca();

	score += testRemoveCauda();

	score += testGetKeys();

	printf("teste list bin: %d/6\n",score);

    if (score == 6)
        return 0;
    else
        return -1;
}

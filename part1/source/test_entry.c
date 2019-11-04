#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**************************************************************/
int testCreate() {
	char *key = strdup("123abc");
	struct data_t *value = data_create2(strlen("1234567890abc")+1, strdup("1234567890abc"));

	struct entry_t *entry = entry_create(key, value);

	int result = (entry->key == key) && 
                     (entry->value == value);

	entry_destroy(entry);

	printf("entry - testCreate: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDup() {
	int result;
	char *key = strdup("123abc");
	struct data_t *value = data_create2(strlen("1234567890abc")+1, strdup("1234567890abc"));

	struct entry_t *entry = entry_create(key, value);

	struct entry_t *entry2 = entry_dup(entry);

	result = entry2 != entry;
	
	result = result && (entry->key != entry2->key) &&
		              (strcmp(entry->key,entry2->key) == 0) && 
                        (entry->value != entry2->value) &&
		              (entry->value->datasize == entry2->value->datasize) &&
                        (memcmp(entry->value->data, entry2->value->data, entry->value->datasize) == 0);

	entry_destroy(entry);
	entry_destroy(entry2);

	printf("entry - testDup: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testDestroy(){
    printf("Módulo entry -> teste entry_destroy:");
	entry_destroy(NULL);
    printf(" passou\n");
	return 1;
}


/**************************************************************/

int main() {
	int score = 0;

	printf("iniciando teste entry bin\n");

	score += testCreate();

	score += testDup();
	
	score += testDestroy();

	printf("teste entry bin: %d/3\n",score);

    if (score == 3)
        return 0;
    else
        return -1;
}

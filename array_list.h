#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_CAPACITY 10
#define ADD_CAPACITY 5

typedef struct{
	char **data;
	int capacity;
	int length;	
} array_list_t;

array_list_t *array_list_new(void);
void freelist(array_list_t *list);
void array_list_add_to_end(array_list_t* l,char *s);

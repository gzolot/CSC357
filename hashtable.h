#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 100

struct DataItem {
   char *value;
   char *key;
};

struct DataItem *search(char *key);
int hashCode(char *key);
void insert(char *key,char *data);
struct DataItem* delete(struct DataItem* item);
void display();

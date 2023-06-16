//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdbool.h>

//#define SIZE 20

//struct DataItem {
//   int data;   
//   int key;
//};
#include "hashtable.h"
struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

int str_to_int(char *key){
	int final = 0;
	for (int i = 0; i<strlen(key); i++){
		final += (int)(key[i]); 
	}
	return final;
}

int hashCode(char *key_str) {
	int key_int = str_to_int(key_str);
	return key_int % SIZE;
}

struct DataItem *search(char *key) {
   //get the hash 
   int hashIndex = hashCode(key);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(!strcmp(hashArray[hashIndex]->key,key))
         return hashArray[hashIndex]; 
			
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insert(char *key,char *value) {

   	struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
	item->value = malloc((sizeof(char)*strlen(value))+1);
	item->key = malloc((sizeof(char)*strlen(key))+1);   	
	strcpy(item->value, value);
	strcpy(item->key, key);

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && strcmp(hashArray[hashIndex]->key,"-1")) {
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }
	
   hashArray[hashIndex] = item;
}

struct DataItem* delete(struct DataItem* item) {
   char *key = item->key;

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty
   while(hashArray[hashIndex] != NULL) {
	
      if(!strcmp(hashArray[hashIndex]->key, key)) {
         struct DataItem* temp = hashArray[hashIndex]; 
			
         //assign a dummy item at deleted position
         hashArray[hashIndex] = NULL; 
         return temp;
      }
		
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }      
	
   return NULL;        
}

void display() {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
	
      if(hashArray[i] != NULL)
         printf(" (%s,%s)",hashArray[i]->key,hashArray[i]->value);
      else
         printf(" ~~ ");
   }
	
   printf("\n");
}

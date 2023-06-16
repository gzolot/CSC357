#include "array_list.h"

array_list_t *array_list_new(){
	array_list_t *l = malloc(sizeof(array_list_t));
	if (l==NULL){
		return NULL;
	}
	l->data = malloc(INITIAL_CAPACITY * sizeof(char*));
	if (l->data == NULL){
		free(l);	
		return NULL;
	}
	l->capacity = INITIAL_CAPACITY;
        l->length = 0;
	return l;
}

void array_list_add_to_end(array_list_t* l,char* s){
	if (l->length == l->capacity){
		l->capacity += ADD_CAPACITY;
		l->data = realloc(l->data,(l->capacity)*sizeof(char*));
	}
	//printf("length=%zu\n",l->length);
	l->data[l->length] = malloc((strlen(s)+1)*sizeof(char));
	strcpy(l->data[l->length],s);
	l->length +=1;
}

//void array_list_add_to_end(array_list_t* l,struct DataItem *s){
//        if (l->length == l->capacity){
//                l->capacity += ADD_CAPACITY;
//                l->data = realloc(l->data,(l->capacity)*sizeof(struct DataItem *));
//        }
//        //printf("length=%zu\n",l->length);
//        l->data[l->length] = malloc(sizeof(struct DataItem));
//	l->data[l->length]->data = 
//        strcpy(l->data[l->length],s);
//        l->length +=1;
//}

void freelist(array_list_t *list){
        for (int i = 0; i < list->length; i++){
                //printf("freed=%s\n", list->data[i]);
                free(list->data[i]);
        }
        free(list->data);
        free(list);

}

//int main(){
//	array_list_t *kevin = array_list_new();
//	char* child = "kevinson";
//	array_list_add_to_end(kevin,child);
//	printf("capacity=%d\n",kevin->capacity);
//	printf("1st elem = %s\n", kevin->data[0]);
//	for (int i = 1; i< 11;i++){
//		array_list_add_to_end(kevin,child);
//		printf("capacity=%d\n",kevin->capacity);
//		printf("%dst elem = %s\n",i+1, kevin->data[i]);
//	}

	
//}

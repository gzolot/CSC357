
#include "kvstore.h"
extern struct DataItem* hashArray[SIZE];
extern struct DataItem* dummyItem;
extern struct DataItem* item;

char* oneWord(char *string, int wordnum){
        int current = isspace(string[0]) ? 0 : 1;
        int prev = 0;
        char* first = (char*) malloc(strlen(string)+1);
        //printf("first=%s\n",first);
        if (first == NULL) {
                return NULL; // handle allocation failure
        }
        *first = '\0';
        char *p1 = first;
        char *p2 = string;
        int flag = 0;
        while ((flag <= wordnum) && (*p2 != '\0')){
                //printf("current=%d, prev=%d\n",current, prev);
                if (current && !prev){
                        flag += 1;
                        //printf("*p2=%c, *p1=%c, flag=%d\n", *p2, *p1, flag);
                }
                if ((flag == wordnum) && current){
                        *p1= *p2;
                        //printf("*p1=%c, *p2=%c\n", *p1, *p2);
                        p1 += 1;
                        //printf("first1=%s\n",first);
                }
                prev = current;
                p2 += 1;
                current = isspace(*p2) ? 0 : 1;
                //printf("next=%d, prev = %d", current, prev);
        }
        *p1 = '\0';
        return first;
}

void database_hash(char *db_name){
	char buffer[100];
	FILE *db = fopen(db_name, "r");
	while (fgets(buffer, sizeof(buffer), db) != NULL){
		char *comma;
		buffer[strlen(buffer)-1]= '\0';
		for (int i = 0; i < strlen(buffer); i++){
			if(buffer[i] == ','){
				comma = &buffer[i];
				buffer[i] = '\0';
			}
		}
		char *key = buffer;
		//printf("key: %s\n",key);
		//array_list_add_to_end(keys, key);
		char *value = (comma+1);
		//printf("value: %s", value);
		//array_list_add_to_end(values, value);
		insert(key,value);
	}
	fclose(db);	
}

void add_to_db(char *db_name, char *key, char *value, int write){
	FILE *file;
	if(write){
		file = fopen(db_name, "w");
	}
	else{
		file = fopen(db_name, "a");
	}
	int size = strlen(key) + strlen(value);
	char buffer[size+3];
	strcpy(buffer, key);
	strcat(buffer, ",");
	strcat(buffer,value);
	strcat(buffer,"\n\0");
	fprintf(file, "%s", buffer);
	fclose(file);
}

static int end_flag = 0;

void signal_handler(int signo){
	if (signo == SIGQUIT){
		end_flag = 1;
	}
}

int main(int argc, char *argv[]){
	if(argc != 3){
		printf("two arguments required");
		return 1;
	}
	if (signal(SIGQUIT,signal_handler) == SIG_ERR){
          	printf("could not process SIGQUIT\n");
        }
	char *db_name = argv[1];
	FILE *database = fopen(argv[1], "r+");
	if (database == NULL){
		printf("%s not found as database file\n", argv[1]);
		return 1;
	}
	fclose(database);
	//array_list_t *keys = array_list_new();
	//array_list_t *values = array_list_new();
	database_hash(db_name);
	//display();
	//printf("key: e, value: %s\n", search("e")->value);
	//for (int i = 0; i < keys->length; i++){
	//	printf("key = %s, value = %s\n", keys->data[i], values->data[i]);
	//}
	//freelist(keys);
        //freelist(values);
	char *fifo_file = argv[2];
	struct dirent *entry;
	DIR *currentdir = opendir(".");
	int fifo_exists=0;
	while((entry = readdir(currentdir)) != NULL){
		if(!strcmp(entry->d_name, fifo_file)){
			fifo_exists = 1;
		}
	
	}
	closedir(currentdir);
	if (!fifo_exists){
		if (mkfifo(fifo_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) != 0){
			printf("could not make fifo: %s\n",fifo_file);
			return 1;
		}
	}
	char buffer[100];
	struct DataItem *thing;
	buffer[0] = '\0';
	while(!end_flag){
		int fd = open(fifo_file, O_RDONLY);
		if (end_flag){
			close(fd);
			continue;
		}
		if (fd == -1) {
        		printf("Failed to open FIFO file for writing.\n");
        		return 1;
    		}
		ssize_t bytes_read = read(fd, buffer, 99);
		if(bytes_read == -1){
			printf("error reading fifo\n");
			close(fd);
		}
		else if(bytes_read == 0){
			//printf("zero bytes read from fifo\n");
			close(fd);
		}
		else{
			buffer[bytes_read] = '\0';
			close(fd);
			char *first = oneWord(buffer, 1);
			char *second = oneWord(buffer,2);
			if (!strcmp(first,"get")){
				thing = search(second);
				char *client_fifo = oneWord(buffer, 3);
				//printf("key: %s\n", first);
				fd = open(client_fifo, O_WRONLY);
				if(fd == -1){
                        		printf("could not open fifo file %s\n", client_fifo);
					close(fd);
                        		return 1;
                		}
				if(thing != NULL){
					if((write(fd, thing->value, strlen(thing->value)))==-1){
                        			printf("Failed to write to FIFO file.\n");
					}
				}
				else{
					if((write(fd, "bummer", strlen("bummer")))==-1){
                                                printf("Failed to write to FIFO file.\n");
                                        }
				}
				close(fd);
				free(client_fifo);
			}
			else{
				char *value = &buffer[strlen(first)+1];
				thing = search(first);
				if(thing == NULL){
					//printf("inserting key: %s, value: %s into database\n",first, value);
					//fflush(stdout);
					//add_to_db(db_name, first, value);
					insert(first, value);
					//display();
					//display();
				}
				else{
					struct DataItem *removing = delete(thing);
					free(removing->key);
					free(removing->value);
					free(removing);
					insert(first,value);
					//printf("key already exists\n");
					//fflush(stdout);
				}	
			}
                	//array_list_add_to_end(keys, first);
                	free(first);
                	//array_list_add_to_end(values, second);
                	free(second);
			//printf("data recieved from buffer: %s\n", buffer);
			
		}	
	}
	int first=0;
	for (int i = 0; i < SIZE; i++){
		if (hashArray[i] != NULL){
			if(!first){
				first = 1;
				add_to_db(db_name, hashArray[i]->key, hashArray[i]->value, 1);
			}
			else{
				add_to_db(db_name, hashArray[i]->key, hashArray[i]->value,0);
			}
			//printf("hash: %s,%s\n", hashArray[i]->key, hashArray[i]->value);
			free(hashArray[i]->key);
			free(hashArray[i]->value);
			free(hashArray[i]);
		}
	}
	//fclose(database);
	//freelist(keys);
	//freelist(values);
	return 0;
}

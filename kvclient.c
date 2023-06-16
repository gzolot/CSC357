#include "kvclient.h"

int last_char(char *str){
	int i=0;
	while(str[i]!='\0'){
		i++;
	}
	return (i-1);
}

void strgs_to_str(char **strgs, char *str, int start){
	int i = start;
	while (strgs[i]!=NULL){
		printf("last char: %c\n", strgs[i][strlen(strgs[i])-1]);
		if(i == start){
			printf("adding %s\n", strgs[i]);
			strcat(str,&strgs[i][1]);
		}
		else if(strgs[i][strlen(strgs[i])-1] == '"'){
			strgs[i][strlen(strgs[i])-1] = '\0';
			printf("adding %s\n", strgs[i]);
			strcat(str,strgs[i]);
		}
		else{
			printf("adding %s\n", strgs[i]);
			strcat(str,strgs[i]);
		}
		i++;
	}
	
}

int main(int argc, char *argv[]){
//	if (argc < 4 || argc > 5){
//		printf("incorrect number of arguments\n");
//		return 1;
//	}
	char *fifo_file = argv[1];
	
	int get= 0;
	int set = 0;
	char *key = argv[3];
	if(!strcmp(argv[2], "get")){ get = 1;}
	else if(!strcmp(argv[2], "set")){set = 1;}
	else{
		printf("second argument is get or set\n");
		return 1;
	}
	if (set){
		if (argc < 5){
			printf("value field required\n");
			return 1;
		}
		char *value = argv[4];
		char message[100];
		strcpy(message,key);
		strcat(message, " ");
		strcat(message, value);
		int fd = open(fifo_file, O_WRONLY);
		if(fd == -1){
			printf("could not open fifo file %s\n", fifo_file);
			return 1;
		}
		if(write(fd, message, strlen(message)) == -1){
			printf("Failed to write to FIFO file.\n");
        		close(fd);
        		return 1;
		}
		close(fd);
	}
	else if(get){
		pid_t pid = getpid();
		char client_fifo[50];
		sprintf(client_fifo, "client_%d_fifo",pid);
		int mkfifo_res = mkfifo(client_fifo, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                if (mkfifo_res == -1){
                        if(errno != EEXIST){
                                printf("could not open client fifo\n");
                        }
                }
		char message[100];
		sprintf(message, "get %s %s", key, client_fifo);
		if (argc != 4){
			printf("incorrect number of aguments");
		}
		int fd = open(fifo_file, O_WRONLY);
		if(fd == -1){
                        printf("could not open fifo file %s\n", fifo_file);
                        return 1;
                }
		
		if((write(fd, message, strlen(message)))==-1){
			printf("Failed to write to FIFO file.\n");
                        close(fd);
                        return 1;
		}
		close(fd);
		fd = open(client_fifo, O_RDONLY);
		if (fd == -1) {
                        printf("Failed to open FIFO file for writing.\n");
                        return 1;
                }
		char buffer[100];
                ssize_t bytes_read = read(fd, buffer, 99);
                if(bytes_read == -1){
                        printf("error reading fifo\n");
                }
		else if(bytes_read == 0){
                        printf("zero bytes read from fifo\n");
                }
		else{
			buffer[bytes_read] = '\0';
			if(strcmp(buffer,"bummer"))
				printf("%s\n",buffer);
			else{
				printf("Key %s does not exist\n",key);
			}
		}
		close(fd);
	}
	return 0;
		
}

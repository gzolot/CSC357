#include "httpd.h"

int sock_fd, newsock = 10000;

void signal_handler(int signo){
        if (signo == SIGQUIT){
                //end_flag = 1;
		shutdown(sock_fd, SHUT_RDWR);
    		shutdown(newsock, SHUT_RDWR);
		exit(0);
        }
}

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

void send_HTTP_response(int socket, char *http_type, char *response_type, char *file_path, char *content, char *fifo_file){
	char send_buffer[BUFFER_SIZE*2] = "";
	strcat(send_buffer, http_type);
	int kv = 0;
	char key[20]= "";
	if(!strcmp(response_type, "GET") || !strcmp(response_type, "HEAD")){
		//printf("inside GET condition\n");
            	FILE *file = fopen(file_path, "r");
		printf("file: %s\n", file_path);
		fflush(stdout);
             	if (file == NULL){
			//printf("0: %c, 1: %c\n", file_path[0], file_path[1]);
			//fflush(stdout);
			if (file_path[0] == 'k' && file_path[1] == 'v'){
				kv = 1;
				strcpy(key, &file_path[3]);
				//printf("key = %s\n", key);
				//fflush(stdout);	
			}
			else{
				strcat(send_buffer, " 404 Not Found\r\n"); //Content-Type: text/html\r\n");
				//strcat(send_buffer, "Content-Length: 0\r\n");  // Set the content length (0 in this case)
            			strcat(send_buffer, "\r\n"); 
				printf("print_buffer: %s\n", send_buffer);
				fflush(stdout);
                 		if(send(socket, send_buffer, strlen(send_buffer), 0)==-1){
				 	printf("could not send send_buffer\n");
				}
			}
           	}
              	else{
                 	struct stat file_info;
                     	if (stat(file_path, &file_info) ==0){
				strcat(send_buffer, " 200 OK\r\nContent-Type: text/html\r\nContent-Length: ");
				char content_length_buff[50];
				sprintf(content_length_buff, "%ld", file_info.st_size);
				strcat(send_buffer, content_length_buff);
				strcat(send_buffer, "\r\n\r\n");
				char text_buffer[BUFFER_SIZE];
				if(!strcmp(response_type, "GET")){
					while(fgets(text_buffer,BUFFER_SIZE, file)){
						strcat(send_buffer, text_buffer);
					}
				}
				printf("send_buffer:\n%s\n", send_buffer);
				if(send(socket, send_buffer, strlen(send_buffer), 0) == -1){
					printf("could not send send_buffer\n");
				}	
                  	}
			else{
				strcat(send_buffer, " 400 Bad Request\r\n"); 	
                       		if(send(socket, send_buffer, strlen(send_buffer), 0) == -1){
                                   	printf("could not send send_buffer\n");
                                }
                   	}
          	}
	}
	if (!strcmp(response_type, "PUT") || kv == 1){
		strcpy(key, &file_path[3]);
		//printf("key: %s, content: %s, fifo: %s\n", key, content, fifo_file);
		strcat(send_buffer, " 200 OK\r\nContent-Length: 0\r\n\r\n");
		if(send(socket, send_buffer, strlen(send_buffer), 0) == -1){
                  	printf("could not send send_buffer\n");
               	}
		if(kv){
                        execlp("./kvclient", "./kvclient", fifo_file, "get", key, NULL);
                }
		else{	
			execlp("./kvclient", "./kvclient", fifo_file, "set", key, content, NULL);
		}
	}
}


int main(int argc, char *argv[]){
	int port;
	char fifo_file[10]= "";
	if (argc == 3){
		port = atoi(argv[2]);
		strcpy(fifo_file,argv[1]);
	}
	else{
		printf("incorrect number of arguments\n");
		return 1;
	}
	if (signal(SIGQUIT,signal_handler) == SIG_ERR){
                printf("could not process SIGQUIT\n");
        }
	//int mlen = 10000;
    	struct sockaddr_in sa, newsockinfo, peerinfo;
    	socklen_t len;
    	
	//char localaddr[INET_ADDRSTRLEN], peeraddr[INET_ADDRSTRLEN], buff[MAXLEN+1];
	sock_fd = socket(AF_INET, SOCK_STREAM,0);
	sa.sin_family = AF_INET;
    	sa.sin_port = htons(port);
    	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sock_fd, (struct sockaddr *) &sa, sizeof(sa));
	listen(sock_fd, DEFAULT_BACKLOG);
	
	while (1){
		len = sizeof(newsockinfo);
		newsock = accept(sock_fd, (struct sockaddr *) &peerinfo, &len);
		pid_t pid = fork();
		if(pid == 0){
			char buffer[BUFFER_SIZE];
			int bytes_read;

			bytes_read = recv(newsock, buffer, BUFFER_SIZE - 1, 0);
			if (bytes_read == -1) {
    				printf("could not read data\n");
			} 
			else if (bytes_read == 0) {
    				printf("Connection closed by the other end\n");
			} 
			else {
   		 		buffer[bytes_read] = '\0';
				printf("%s",buffer);
				char* content_start = strstr(buffer, "\r\n\r\n") + 4;  // Find the start of the content
    				//printf("Content: %s\n", content_start);
				char *response_type = oneWord(buffer, 1);
				char *file_path = oneWord(buffer, 2)+1;
				char *http_type = oneWord(buffer, 3);
				//printf("response_type:%s\nfile_path:%s\n", response_type, file_path);
				//fflush(stdout);
				send_HTTP_response(newsock, http_type, response_type, file_path, content_start, fifo_file);
				free(response_type);
				free(file_path-1);
				free(http_type);
			}
			close(newsock);
			//if (close(newsock) == -1) {
        		//	perror("Error closing socket");
        		//	return 1;
    			//}
			exit(0);
		}
	}
		

	return 0;
}

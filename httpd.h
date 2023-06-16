#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <signal.h>

#define MAXLEN 1000
#define DEFAULT_BACKLOG 100
#define BUFFER_SIZE 200


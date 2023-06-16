CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
MAIN = httpd
SECOND = kvclient
THIRD = kvstore
CLIENT = kvclient.o
STORE =  kvstore.o array_list.o hashtable.o
OBJS = httpd.o

all : $(MAIN) $(SECOND) $(THIRD)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

$(SECOND) : $(CLIENT)
	$(CC) $(CFLAGS) -o $(SECOND) $(CLIENT)

$(THIRD) : $(STORE)
	$(CC) $(CFLAGS) -o $(THIRD) $(STORE)

kvclient.o : kvclient.c kvclient.h
	$(CC) $(CFLAGS) -c kvclient.c

kvstore.o : kvstore.h kvstore.c
	$(CC) $(CFLAGS) -c kvstore.c

array_list.o : array_list.c array_list.h
	$(CC) $(CFLAGS) -c array_list.c

hashtable.o : hashtable.c hashtable.h
	$(CC) $(CFLAGS) -c hashtable.c

httpd.o : httpd.c httpd.h
	$(CC) $(CFLAGS) -c httpd.c

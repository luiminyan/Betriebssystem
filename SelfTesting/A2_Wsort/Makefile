CC = gcc
CFLAGS= -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: wsort

wsort: wsort.o
	$(CC) -o wsort wsort.o

wsort.o: wsort.c
	$(CC) $(CFLAGS) -c wsort.c
	
clean: 
	rm -f wsort *.o

.PHONY: all clean
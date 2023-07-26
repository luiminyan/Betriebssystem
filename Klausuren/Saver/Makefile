CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: saver

saver: saver.o
	$(CC) -o saver saver.o

saver.o: saver.c
	$(CC) $(CFLAGS) -c saver.c

clean: 
	rm -f saver.o saver
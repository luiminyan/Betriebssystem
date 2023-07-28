CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: parrots

parrots: parrots.o sem.o queue.o triangle.o
	$(CC) -o parrots parrots.o sem.o queue.o triangle.o

parrots.o: parrots.c sem.h queue.h triangle.h
	$(CC) $(CFLAGS) -c parrots.c
	
clean:
	rm -f parrots.o parrots
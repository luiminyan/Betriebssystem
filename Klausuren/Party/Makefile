CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: party

party: party.o
	$(CC) -o party party.o

party.o: party.c
	$(CC) -c party.c

clean:
	rm -f party party.o
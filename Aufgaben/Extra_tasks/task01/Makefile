CFLAGS=-Wall -Werror -std=c11 -pedantic -D_XOPEN_SOURCE=700
CC=gcc

.PHONY: all test clean

all: printf test

test:
	./test

clean:
	rm -f printf printf.o

printf: printf.o
	gcc $^ -o $@

printf.o: printf.c
	gcc $^ -c -o $@

CFLAGS=-Wall -Werror -std=c11 -pedantic -D_XOPEN_SOURCE=700
CC=gcc

.PHONY: all test clean

all: test
	./test

clean:
	-rm -f test strings.o

test: test.o strings.o
	gcc $^ -o $@

strings.o: strings.c
	gcc $^ -c -o $@

CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: test test-ref

test: test.o halde.o
	$(CC) -o test test.o halde.o

test.o: test.c halde.h
	$(CC) $(CFLAGS) -c test.c

halde.o: halde.c halde.h
	$(CC) $(CFLAGS) -c halde.c

test-ref: test.o halde-ref.o
	$(CC) -o test-ref test.o halde-ref.o

clean: 
	rm -f test test-ref test.o halde.o

.PHOENY: all clean


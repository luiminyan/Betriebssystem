#define markos
CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

prog: main.o fib.o	#to make the executable file 'prog', main.o and fib.o are needed
	$(CC) $(CFLAGS) -o prog main.o fib.o

main.o: main.c fib.h		#main.c uses fib.h, so both are related
	$(CC) $(CFLAGS) -c main.c		#create operable file: main.o

fib.o: fib.c fib.h	#need to relate itself(fib.h)
	$(CC) $(CFLAGS) -c fib.c

clean:
	rm -f *.o prog	#clean / delete the files

.PHONY: prog clean

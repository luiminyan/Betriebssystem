CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

mytool:main.o tool1.o tool2.o
	$(CC) -o mytool main.o tool1.o tool2.o 

main.o:main.c
	$(CC) $(CFLAGS) -c main.c

tool1.o:tool1.c
	$(CC) $(CFLAGS) -c tool1.c

tool2.o:tool2.c
	$(CC) $(CFLAGS) -c tool2.c

clean:
	rm -f *.o mytool 

.PHONY: all clean
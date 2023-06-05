CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700	#compiler flags

all: clash		#define "all": compile the clash file

#gcc -o <name> name the executable file as clash (-o clash)
clash: clash.o plist.o		#"clash": need clash.o and plist.o
	$(CC) -o clash clash.o plist.o

#gcc -c xxx.c, create file xxx.o 
clash.o: clash.c plist.h	#need clash.c and plist.h (included in clash.c) 
	$(CC) $(CLAFGS) -c clash.c

plist.o: plist.c plist.h	#need plist.c and plist.h (included in plist.c)
	$(CC) $(CLAFGS) -c plist.c

clean:					#remove clash and all files of type o
	rm -f clash *.o

.PHONY: all clean

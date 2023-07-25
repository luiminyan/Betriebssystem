CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

all: hupsi

hupsi: hupsi.o
	$(CC) -o hupsi hupsi.o

hupsi.o: hupsi.c
	
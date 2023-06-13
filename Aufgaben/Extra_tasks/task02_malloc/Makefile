.PHONY: all
all: malloc.o

malloc.o: malloc.c
	gcc $^ -c -o $@

.PHONY: clean
clean:
	-rm -f test malloc.o

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv) {
    char a[] = "dire";
    char b[] = "cto";
    char c[] = "ry";
    char* buffer = malloc(sizeof(char*));
    sprintf(buffer, "%s/%s/%s", a, b, c);
    printf("%s\n", buffer);
    printf("size of string = %lu\n", strlen(buffer));
    printf("%c\n", buffer[10]);
}
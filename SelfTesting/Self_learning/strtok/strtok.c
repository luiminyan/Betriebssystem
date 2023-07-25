#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static void die (char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void print_list(char** arr, int len){
    for (int j = 0; j < len; j++) {
        if (printf("%s\n", arr[j]) < 0) {
            die("printf");
        }
    }

    //fflush
    if (fflush(stdout) == EOF) {
        die("fflush");
    }
}

int main (int argc, char* argv[]) {
    char input[25] = "ls \t-l \ttmp";

    //make copy by strdup
    char* copy = strdup(input);
    //error by strdup
    if (copy == NULL) {
        die("strdup");
    }

    //create token
    char* token;

    //create buffer to store the splitt buffer
    int buff_len = 50;
    char* buffer[buff_len];

    //iterator
    int i = 0;

    //extract first token, strtok(string, breaker)
    token = strtok(input, " \t");
    while (token) {
        //store splitted token into buffer
        buffer[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }

    //print list
    print_list(buffer, i);

    exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024;
static die(char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

static warn(char* msg){
    fprintf(stderr, "%s\n", msg);    
}

static void read_input(){
    //create buffer
    int buff_len = MAX_LEN;
    char buff[buff_len + 2];

    //read input
    while (1) {
        //fgets, returns a ptr to the string
        if (fgets(buff, buff_len + 2, stdin) == NULL) {
            //if error
            if (ferror(stdin)) {
                die("fgets");
            }
            // EOF
            if (feof(stdin)) {
                break;
            }
        }

        //adjust the strings
        //if the string is too short
        if (strlen(buff) < 2) {
            //only '\n', as '\0' not included in strlen
            warn("String too short!");
        }

        //string too long
        if (strlen(buff) >= buff_len + 1 && buff[buff_len] != '\n') {
            warn("String too long");
            break;
        }
        

    }
}

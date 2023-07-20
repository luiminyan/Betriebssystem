#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>      //errno

#define MAX_LINE 100    //'\n' or '\0' not included

//dealing errno error
static void die (char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

//warn
static void warn (char* msg) {
    fprintf(stderr, "%s\n", msg);
    //no error dealing on error dealing
}
int main(int argc, char* argv[]) {
    while (1) {
        //read input
        //create line buffer
        char line[MAX_LINE + 2];    // +2: '\0' and '\n'
        //read a line input
        if (fgets(line, MAX_LINE + 2, stdin) == NULL) {
            //EOF
            if (feof(stdin)) {
                break;
            }
            //error / ferror(stdin)
            die("fgets");
        }

        //modify line input
        //line too long: line = [MAX_LINE]X && X != '\n'
        if ((strlen(line) == MAX_LINE + 1) && (line[strlen(line) - 1] != '\n')) {
            warn("Input line too long!");
            continue;
        }

        //line too short
        if ((strlen(line) < 2) || line[0] == '\n') {
            warn("Input too short!");
            continue;
        }

        //remove the '\n', if exist
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        
        
    }
    
    

    exit(EXIT_SUCCESS);
}
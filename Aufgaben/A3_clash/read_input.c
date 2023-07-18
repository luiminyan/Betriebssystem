#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      //errno

//purpose of this program: read single line stdin and store in buffer

static void die(char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void warn(char* msg) {
    fprintf(stderr, "%s\n", msg);
}

static void read_input() {
    //create buffer for stdin
    size_t buff_size = 100;
    //1st method: dynamic, changeable size
    //char* buffer = malloc(sizeof(char*) * buff_size);

    //2nd method: static, size not changeable
    char buffer[buff_size + 1];     // +1: for "\0"

    while (1) {
        //fgets
        if (fgets(buffer, buff_size + 1, stdin) == NULL) {
            //special case: no input
            if (ferror(stdin)){
                die("fgets");
            }
            else {
                fprintf(stderr, "End of input.\n");
                if (ferror(stderr)) {
                    die("fprintf");
                }
                // exit(EXIT_SUCCESS);     //quit the program
            }
        }

        //modify the input
        //too short: if only "\n" included
        if (strlen(buffer) < 2) {
            warn("input too short");
            continue;
        }

        //too long
        if (strlen(buffer) == buff_size && buffer[buff_size - 1] == "\n") {
            warn("input too long");
            //skip the chars in stdin
            int c;
            do {
                c = fgetc(stdin);
            } while ( (c != EOF) && (c != "\n"));
            if (ferror(stdin)) {
                die("fgetc");
            }
            continue;
        }

        //remove the "\n"
        if (buffer[strlen(buffer) - 1] == "\n") {
            buffer[strlen(buffer) - 1] = "\0";
        }
    }
    

}


int main(int argc, char** argv) {
    //fflush stdin
    if (fflush(stdin) == EOF) {
        die("fflush");
    }

    read_input();
    
    exit(EXIT_SUCCESS);
}
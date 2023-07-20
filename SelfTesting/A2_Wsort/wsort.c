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

//compare func
static int comp_func(const void* a, const void* b) {
    //casting into string
    const char** ca = (const char**) a;
    const char** cb = (const char**) b;
    return *ca - *cb;
}

int main(int argc, char* argv[]) {
    //create line list buffer
    int line_count = 0;
    int alloc_count = 1;

    //allocation for string array
    char** line_list = calloc(alloc_count, sizeof(char*));
    //error by calloc
    if (line_list == NULL) {
        die("calloc");
    }

    //read line recursivly till EOF of stdin
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
            //skip the rest chars of this line
            int c;
            do {
                c = getchar();
            } while (c != '\n' && c != EOF);
            if (ferror(stdin)) {
                die("getchar");
            }
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

        /* //test the readline func
        printf("%s\n", line); */
        
        //write line into line_list
        if (line_count >= alloc_count) {
            //collected words >= allocation times
            //increase the line_list
            alloc_count *= 2;
            //realloc
            line_list = realloc(line_list, sizeof(char*) * alloc_count);
            //error by realloc
            if (line_list == NULL) {
                die("realloc");
            }
        }

        //write line into line list
        line_list[line_count] = strdup(line);
        //error by strdup
        if (line_list[line_count] == NULL) {
            die("strdup");
        }

        //increase line count / number of collected word
        line_count++;
    }   /* end of the read input while loop*/

    //sort the list
    qsort(line_list, line_count, sizeof(char*), comp_func);

    //write into stdout
    for (int i = 0; i < line_count; i++) {
        if (fputs(line_list[i], stdout) == EOF) {
            die("fputs");
        }
        //free allocation by line
        free(line_list[i]);
    }
    //free total allocation 
    free(line_list);

    //flush
    if (fflush(stdout) == EOF) {
        die("ffllush");
    }
    
    exit(EXIT_SUCCESS);
}

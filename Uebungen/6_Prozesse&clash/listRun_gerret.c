#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Zu wenig Argumente\n");
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // Kindprozess
            execlp(argv[1], argv[1], argv[i], NULL);
            perror("execlp");
            return EXIT_FAILURE;
        } else {
            // Elternprozess
            int status;
            pid_t value = wait(&status);
            if (value == -1) {
                perror("wait");
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}
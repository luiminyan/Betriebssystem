#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 100

static void die(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

static int compare(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

int main(int argc, char const *argv[]) {
    size_t list_length = 0;
    size_t list_capacity = 1;
    char **list = malloc(list_capacity * sizeof(char*));
    if (!list) die("malloc");

    while(1) {
        // Zeile einlesen
        char input_buffer[MAX_WORD_LEN + 2];
        if (!fgets(input_buffer, MAX_WORD_LEN + 2, stdin)) {
            if (feof(stdin)) {
                break; // wir sind am Ende der Eingabe angekommen
            } else {
                die("fgets"); // es gab einen Fehler beim Lesen
            }
        }

        size_t length = strlen(input_buffer);

        // Wenn der Puffer vollständig gefüllt ist und das letzte Zeichen kein
        // '\n' ist, heißt das, dass die aktuelle Zeile über 100 Zeichen hinausgeht.
        // → Fehlermeldung ausgeben, Zeile ignorieren
        if (length == 101 && input_buffer[100] != '\n') {
            fprintf(stderr, "zu lang\n");

            // Wir haben den Rest der Zeile aber noch nicht von der Eingabe gelesen.
            // Also müssen wir zunächst den Rest der Zeile einlesen. Das machen wir
            // hier Zeichen für Zeichen, bis wir mit '\n' am Ende der Zeile
            // angekommen sind.
            int c; // !! muss ein int sein, da EOF nicht in char reinpasst
            do c = getc(stdin); while(c != '\n' && c != EOF); // einlesen, bis '\n' oder EOF gelesen wird
            if (c == EOF && !feof(stdin)) { // Ein EOF kann auch auf einen Fehler hinweisen
                die("getc");
            }

            continue;
        }

        // Leere Zeilen werden ignoriert
        if (input_buffer[0] == '\n') {
            continue;
        }

        // Das '\n' am Ende soll nicht zum Wort gehören,
        // also kürzen wir ggf. den String um ein Zeichen
        if (input_buffer[length - 1] == '\n') {
            input_buffer[length - 1] = '\0';
        }

        // Derzeit haben wir den String noch in einer lokalen Variable auf dem Stack.
        // Damit wir den String in die Liste einfügen können, müssen wir ihn erst ins
        // Heap kopieren. Hier ist strdup sehr nützlich.
        char *stored_string = strdup(input_buffer);
        if (!stored_string) die("strdup");

        // Falls in der Liste kein Platz mehr für den neuen Eintrag ist, müssen
        // wir sie erst verlängern.
        if (list_length >= list_capacity) {
            list_capacity *= 2; // wir wollen die Kapazität verdoppeln
            char **new_list = realloc(list, list_capacity * sizeof(char*));
            if (!new_list) die("realloc");
            list = new_list;
        }
        list[list_length++] = stored_string; // den String an die Liste anfügen...
    }

    // Die Liste mit den Strings sortieren...
    qsort(list, list_length, sizeof(char*), compare);

    // Den Ihnalt der Liste ausgeben und Speicher freigeben...
    for (int i = 0; i < list_length; i++) {
        if (printf("%s\n", list[i]) < 0) die("printf");
        free(list[i]);
    }
    free(list);

    // Flush, um zu erkennen ob es einen Fehler bei der Ausgabe gab
    if (fflush(stdout) != 0) die("fflush");

    return EXIT_SUCCESS;
}

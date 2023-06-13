struct Student {
    char *name;
    int age;
};
typedef struct Student Student;

/**
 * @brief Fügt einen Studenten zur internen Datenstruktur hinzu
 *
 * Speichert die Studenten intern in einem Array ab, welches
 * dynamisch vergrößert wird.
 *
 * @return -1 im Fehlerfall, 0 sonst
 */
int addStudent(const Student *student);

/**
 * @brief Gibt alle Studenten aus
 *
 * Gibt alle Studenten auf der Kommandozeile aus. Die Ausgabe erfolgt in der
 * Form 'Name: <name>, Alter: <alter>'. Jeder Student wird in einer neuen
 * Zeile ausgegeben.
 * 
 * @return -1 bei Schreibfehler, 0 sonst
 */
int printStudent(void);

/**
 * @brief Gibt das gesamte Array frei
 */
void freeStudent(void);

/**
 * @brief Konkatiniert a und b
 *
 * Die Strings a und b werden zu einem neuen, heap-allokierten String zusammengesetzt
 *
 * @return Heap-allokierter String c
 */
char *join(const char *a, const char *b);

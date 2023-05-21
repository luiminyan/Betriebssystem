#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "plist.h"

/* Die Funktionen insertElement() und removeElement() bitte unveraendert lassen!
 * Falls Sie einen Bug in dieser Implementierung finden, melden Sie diesen bitte
 * an bs@lists.ruhr-uni-bochum.de
 */

struct queue_element {
	pid_t pid;
	char *cmdLine;
	struct queue_element *next;
};

static struct queue_element *head;

//walk through the linked list and print the pid and command(print_job)
//callback function = print_job function from clash.c
void walkList(bool (*callback) (pid_t, const char *)) {
	// done
	//iterate all the element in the linked list, start = head, condition = not empty, next = next element
	for (struct queue_element *element = head; element != NULL; element = element -> next){
		if (callback(element->pid, element->cmdLine)){
			break;
		}
	}
}

int insertElement(pid_t pid, const char *cmdLine) {
	struct queue_element *current = head;
	struct queue_element *previous = NULL;

	while (current) {
		if (current->pid == pid) {
			return -1;
		}

		previous = current;
		current = current->next;
	}

	current = malloc(sizeof(struct queue_element));
	if (current == NULL) {
		return -2;
	}

	current->cmdLine = strdup(cmdLine);
	if (current->cmdLine == NULL) {
		free(current);
		return -2;
	}

	current->pid  = pid;
	current->next = NULL;

	/* Einhaengen des neuen Elements */
	if (previous == NULL) {
		head = current;
	} else {
		previous->next = current;
	}

	return pid;
}

int removeElement(pid_t pid, char *buf, size_t buflen) {
	if (head == NULL) {
		return -1;
	}

	struct queue_element *current = head;
	struct queue_element *previsous = NULL;

	while (current) {
		if (current->pid == pid) {
			if (previsous == NULL) {
				head = head->next;
			} else {
				previsous->next = current->next;
			}

			strncpy(buf, current->cmdLine, buflen);
			if (buflen > 0) {
				buf[buflen-1]='\0';
			}
			int retVal = (int)strlen(current->cmdLine);

			/* Speicher freigeben */
			free(current->cmdLine);
			current->cmdLine = NULL;
			current->next = NULL;
			current->pid = 0;
			free(current);
			return retVal;
		}

		previsous = current;
		current = current->next;
	}

	/* PID not found */
	return -1;
}

#include <stdio.h>
#include <stdlib.h>

// TODO: structs, global variables, etc.
typedef struct listElement{
    int value;
    struct listElement *next;
} n;

//empty head element
n *head = NULL;

static int insertElement(int value) {
	// TODO: implement me!
    if (value < 0){
        return -1;
    }
    if (head == NULL){
        head = (n*) malloc(sizeof(n*));
        head->value = value;
        head->next = NULL;
        return head->value;
    }

    n *node = head;

    while (1){
        if (node->value == value){
            return -1;
        }
        if (node->next == NULL){
            n *toInsert = (n*) malloc(sizeof(n*));
            toInsert->value = value;
            toInsert->next = NULL;
            node->next = toInsert;
            return node->next->value;
        }
        node = node->next;
    }
}



static int removeElement(void) {    //remove the newest node
	// TODO: implement me!
    if (head == NULL){      //empty list return -1
        return -1;
    }

    // if only one node
    if (head->next == NULL){
        int result = head->value;
        return result;
    }

    n *newNode1 = head;
    n *newNode2 = head;
    while (newNode1->next != NULL){     //end node
        newNode1 = newNode1->next;
    }
    while (newNode2->next != newNode1){ //node before end node
        newNode2 = newNode2->next;
    }
    int result = newNode1->value;
    free(newNode1);
    newNode2->next = NULL;
    return result;
}

int main (int argc, char* argv[]) {
	printf("insert 47: %d\n", insertElement(47));
	printf("insert 11: %d\n", insertElement(11));
	printf("insert 23: %d\n", insertElement(23));
	printf("insert 11: %d\n", insertElement(11));   //return -1

	printf("remove: %d\n", removeElement());
	printf("remove: %d\n", removeElement());
    printf("remove: %d\n", removeElement());


    // TODO: add more tests
    printf("insert -1: %d\n", insertElement(-1));   //return -1
    printf("insert 1: %d\n", insertElement(1));
    printf("insert 2: %d\n", insertElement(2));
    printf("insert 3: %d\n", insertElement(3));

    exit(EXIT_SUCCESS);
}

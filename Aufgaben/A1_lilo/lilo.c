#include <stdio.h>
#include <stdlib.h>

// TODO: structs, global variables, etc.
typedef struct listeElement{
    int value;
    struct listeElement *next;  //pointer to the next element
} node;

//create empty node head
node *head = NULL;

static int insertElement(int value) {
	// TODO: implement me!
    if (value >= 0){                              //check the value of the node not negative
        if (head == NULL){                        //the list is empty
            head = malloc(sizeof(node));     //implement head node, give head an address
            if (head == NULL){                    //address allocate is not successful
                return -1;
            }
            head->value = value;
            head->next = NULL;                    //empty next node
            return head->value;
        }
        else{
            node *elementInList = head;                   //create a elementInList node initialized with address of head node
            while (1){
                if (elementInList->value == value){       //if value duplicate between input and elementInList, return -1
                    return -1;
                }

                if (elementInList->next == NULL){                     //new node to be inserted at end of the list
                    node *nextNode = malloc(sizeof (node));      //create node nextNode for instance
                    if (nextNode == NULL){
                        return -1;
                    }
                    nextNode->value = value;                          //implementations of the nextNode
                    nextNode->next = NULL;
                    elementInList->next = nextNode;                  //insert nextNode at the end of the list
                    return elementInList->next->value;
                }
                elementInList = elementInList->next;         //elementInList = next node
            }
        }
    }
}

static int removeElement(void) {
	// TODO: implement me!
    //the list is empty
    if (head == NULL){
        return -1;
    }
    else{
        int result = head->value;    //store the value of head in an integer
        node *newElement = head;     //create newElement node, pointing to the head node
        head = head->next;           //head node is now of the address from the next element
        free(newElement);            //free the newElement node (the address of the old head node)
        return result;
    }
}

int main (int argc, char* argv[]) {
	printf("insert 47: %d\n", insertElement(47));       //return 47
	printf("insert 11: %d\n", insertElement(11));       //return 11
	printf("insert 23: %d\n", insertElement(23));       //return 23
	printf("insert 11: %d\n", insertElement(11));       //return -1, duplicate

	printf("remove: %d\n", removeElement());                  //return 47
	printf("remove: %d\n", removeElement());                  //return 11

	// TODO: add more tests
    printf("remove: %d\n", removeElement());                  //return 23
    printf("remove: %d\n", removeElement());                  //return -1
    printf("insert 0: %d\n", insertElement(0));         //return 0
    printf("insert 1: %d\n", insertElement(1));         //return 1
    printf("insert 2: %d\n", insertElement(2));         //return 2
    printf("remove: %d\n", removeElement());                  //return 0
    printf("remove: %d\n", removeElement());                  //return 1


    exit(EXIT_SUCCESS);
}

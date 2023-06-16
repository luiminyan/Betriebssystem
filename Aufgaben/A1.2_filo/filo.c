#include <stdio.h>
#include <stdlib.h>	//malloc, free


// TODO: structs, global variables, etc.
//define struct
typedef struct listelement{
	/* data */
	int value;
	struct listelement* next;
} node;			//shorten for struct listelement

//global variables
static node* head;		//head node = NULL


static int insertElement(int value) {
	// TODO: implement me!
	if (value < 0){			//invalid value, fehler
		return -1;
	}

	if (head == NULL){
		head = malloc(sizeof(node*));

		if (head == NULL){
			return -1;
		}

		head->next = NULL;
		head->value = value;
		return head->value;
	}

	node* listNode = head;
	while(1){
		if (listNode->value == value){
			return -1;
		}
		if(listNode->next == NULL){
			node* nextNode = malloc(sizeof(node*));
			if (nextNode == NULL){
				return -1;
			}
			nextNode->value = value;
			nextNode->next = NULL;
			listNode->next = nextNode;
			return listNode->next->value;
		}
		listNode = listNode->next; 	
	}
}

static int removeElement(void) {
	// TODO: implement me!
	if (head == NULL){
		return -1;
	}
	node* listNode = head;	

	return -1;
}

int main (int argc, char* argv[]) {
	printf("insert 47: %d\n", insertElement(47));
	printf("insert 11: %d\n", insertElement(11));
	printf("insert 23: %d\n", insertElement(23));
	printf("insert 11: %d\n", insertElement(11));
	printf("insert -2: %d\n", insertElement(-2));

	printf("remove: %d\n", removeElement());
	printf("remove: %d\n", removeElement());

	// TODO: add more tests
	exit(EXIT_SUCCESS);
}

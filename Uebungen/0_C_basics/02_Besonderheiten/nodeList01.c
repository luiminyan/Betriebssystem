#include <stdio.h>

//def structure ListNode
typedef struct ListNode{
	int data;	//the data of the node
	struct ListNode* next;	//ptr to the next node of type (struct ListNode)
} node_t;	//node_t is the shorten of struct ListNode


int main(int argc, char* argv[]){
	//define the head node with ptr
	node_t* head = NULL;	//head is now empty
	
	//create nodes
	node_t node0 = {.data = 42, .next = NULL};
	node_t node1 = {.data = 1337, .next = &node0};
	node_t node2 = {.data = -23, .next = &node1};
	//node2 -> node1 -> node0
	
	head = &node2;	//connect head with node2
	//head = node2 -> node1 -> node0
	
	//test correctness
	printf("head = %d\n", head->data);
	printf("next = %d\n", head->next->data);
	printf("next = %d\n", head->next->next->data);
	printf("next = %d\n", head->next->next->next->data);
}




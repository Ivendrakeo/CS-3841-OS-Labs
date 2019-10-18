#include <stdlib.h>
#include "llist.h"
#include <stdio.h>

int main(void){
	printf("hello world\n");

	printf("-------------------------------------------\n");
	
	list myList;
	llInit(&myList);
	printf("initalized...\n");
	llPushFront(&myList, "one");
	llRemove(&myList, getNode(&myList,0));	
	llPushFront(&myList, "two");
	llPushFront(&myList, "three");
	llPushBack(&myList, "Four");
	llPushBack(&myList, "Five");
	printList(&myList);
	printHeadAndTail(&myList);
	printf("-------------------------------------------\n");
	llRemove(&myList, getNode(&myList,2));
	printList(&myList);
	printHeadAndTail(&myList);
	llClear(&myList);
	printList(&myList);
	printHeadAndTail(&myList);	
}

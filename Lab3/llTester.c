/*
1.All of the list functions need a “reference” to the list structure, and according to this design, that list reference
is passed as a pointer. Why is this necessary? Do all of the list functions need this to be passed as a pointer?
Any exceptions? Be specific in your answer.

All the list functions that we used in this lab required passing their arguments by pointer. In our case, the list
specifically needed to be passed by pointer because it was an instance of a struct and in order to avoid duplicating
the contents of the struct on the stack (passing by value) a pointer was needed. The same argument is also made for
passing nodes. Another side benefit of passing node structs by reference is that we can determine if their data is
equal by simple equality (node1 == node2?) which is convenient for our use. We also had to pass strings by pointer
because of the same aforementioned benefits but also because we don’t know the length of the string, and a pointer
will allow us to avoid that conundrum because C style strings are null terminated.
Overall, because the data we were passing were all either structs, or strings, pointers were preferred. If the data
were simple types such as characters or integers, pointers would have been unnecessary.

2.Unlike a Java or C++ implementation, this implementation cannot “hide” any of the internal structure of the list.
That is, users of the list could mess up the next and prev pointers if they are careless. Can you think of any way we
could hide the structure of the list to lessen the chances a user will mess up the list? Describe in brief detail.

Since there are no objects in C, you cannot embed the list in any wrappers. You could make its fields constant which
would make it so the user could not edit them, but that would also not allow methods to edit them either.
Given these considerations C is just a language that places a lot of trust in the user and is under the assumption
that they know what they are doing and will make educated decisions. Therefore there may be a way to “hide” the
structure of the list but it’s not obvious or seemingly straightforward.

3.What if all llClear() did was assign NULL to head and tail in the list structure and nothing else. Would the program
crash? Would there be any side effects? Try it and report results.

The program would not crash nor would there be any immediately noticeable side effects. Instead, the nodes and
associated strings would be lost in memory leaks which if left running over a long period of time with many resets
may result in the heap running out of allocable memory. This is why it’s important to “free the malloc’s”.

4.This design requires the user to iterate the list somewhat manually as demonstrated in the sample driver. Propose the
design of an iterator for this list. What data items would the iterator need to store (in a structure, perhaps)? What
functions would the iterator supply?

The iterator could store the current position within the list as well as the node associated with that position.
Methods to set the position and increment/decrement the position would be useful. The intended usage would be easier
access to getting nodes within the list.

My Experience with the lab:

I think this lab went fairly well. Thankfully most of the code I wrote worked the first time without needed extensive
debugging. There were a few times where I caught edge cases that I forgot to handle which I had to go back and fix
(such as properly resetting the head and tail when removing the last element). Some of the helper methods I wrote to
verify operation did throw me for a loop since they were giving me output which was correct but I was misinterpreting
and that caused me to waste some time trying to debug working code. Valgrind was useful for checking the memory and gdp
was also useful for checking exactly which methods were causing errors.
As for working with Dynamic memory, I was familiar with it before so implementation was fairly quick, though I did have
to look up the syntax for how to allocate enough memory for a struct.

 */

#include <stdlib.h>
#include "llist.h"
#include <stdio.h>

/*
 * An iterator allows for easy traversal over a list.
 * It requires an associated list reference to opperate.
 * the node value and index value retain where in the list the iterator is currently at.
 */
typedef struct iterator {
	node *node;
	int index;
	list *list;
} iterator;

/*
 * sets up the iterator for the supplied list starting at the head
 */
void initIterator(iterator* itr, list* myList){
	itr->node = myList->head;
	itr->index = 0;
	itr->list = myList;
}

/*
 * Gets the value of the node at the iterator's current location
 */
char * getIteratorValue(iterator* itr){
	return itr->node->string;
}

/*
 * Gets the current index of the iterator
 */
int getIteratorIndex(iterator* itr){
	return itr->index;
}

/*
 * increments the iterator to the next node.
 * returns non-zero if sucessful and 0 if next node is NULL
 */
int next(iterator* itr){
	if(itr->node->next != NULL){
		itr->node = itr->node->next;
		itr->index++;
		return 1;
	} else {
		return 0;
	}
}

/*
 * decrements the iterator to the previous node.
 * returns non-zero if sucessful and 0 if next node is NULL
 */
int previous(iterator* itr){
	if(itr->node->prev != NULL){
		itr->node = itr->node->prev;
		itr->index--;
		return 1;
	} else {
		return 0;
	}
}

/*
 * Set's the iterator to the desired position in the list.
 * returns non-zero if sussful and 0 if out of bounds.
 * if return is zero, iterator will not change
 */
int setPosition(iterator* itr, int position){
	if(position > llSize(itr->list))
		return 0;
	if(itr->index != position){
		if(itr->index < position){
			while(itr->index < position){
				if(itr->node->next != NULL){
					itr->node = itr->node->next;
					itr->index++;
				} else {
					return 0;
				}
			}
		} else {
			while(itr->index > position){
				if(itr->node->prev != NULL){
					itr->node = itr->node->prev;
					itr->index--;
				} else {
					return 0;
				}
			}
		}
	}
	return 1;
}

static void printList(list* myList){
	node* workingNode = myList->head;
	int size = 0;
	printf("[list data]\n");
	while(workingNode != NULL){
		printf("-node: %d val: %s\n",size,workingNode->string);
		size++;
		workingNode = workingNode->next;
	}
}

static void printHeadAndTail(list* myList){
	if(!((myList->head == NULL) || (myList->tail == NULL)))
		printf("head: %s, tail: %s\n",myList->head->string,myList->tail->string);
	else
		printf("head: NULL, tail: NULL\n");
}

static node * getNode(list* myList, int position){
	node* nodeWalker = myList->head;
	for(int i =0;i<position;i++){
		nodeWalker = nodeWalker->next;
	}
	return nodeWalker;
}

int main(void){
    printf("------BEGIN NORMAL TESTS-----\n");
	list myList;
	llInit(&myList);
	printf("initalized...\n");
	llPushFront(&myList, "one");
	printf("TEST 1: testing one element...\n");
	printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST 2: removing element...\n");
	llRemove(&myList, getNode(&myList,0));
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST 3: creating list of 5 elements...\n");
	llPushFront(&myList, "two");
	llPushFront(&myList, "one");
	llPushBack(&myList, "three");
	llPushBack(&myList, "four");
    llPushBack(&myList, "five");
    printf("-size: %d\n",llSize(&myList));
	printList(&myList);
	printHeadAndTail(&myList);
    printf("TEST 4: popping head and tail nodes\n");
    llPopBack(&myList);
    llPopFront(&myList);
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST 5: recreating list of 5 and removing middle node\n");
    llPushFront(&myList, "one");
    llPushBack(&myList, "five");
	llRemove(&myList, getNode(&myList,2));
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST: 6 removing head and tail node\n");
    llRemove(&myList, getNode(&myList,3));
    llRemove(&myList, getNode(&myList,0));
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST 7: clearing list\n");
	llClear(&myList);
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("------NORMAL TESTS COMPLETE-----\n");
    printf("\n");
    printf("-----BEGIN ITERATOR TESTS------\n");
    printf("Initalize list of 5 elements\n");
    llPushFront(&myList, "two");
    llPushFront(&myList, "one");
    llPushBack(&myList, "three");
    llPushBack(&myList, "four");
    llPushBack(&myList, "five");
    printf("-size: %d\n",llSize(&myList));
    printList(&myList);
    printHeadAndTail(&myList);
    printf("TEST 0: Initalize Iterator...\n");
    iterator itr;
    initIterator(&itr,&myList);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 1: increment iterator\n");
    next(&itr);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 2: decrement iterator\n");
    previous(&itr);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 2.5: decrement below range\n");
    previous(&itr);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 3: set position to 3\n");
    setPosition(&itr,3);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 4: set position to out of bounds\n");
    setPosition(&itr,10);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("TEST 4: set position back to zero\n");
    setPosition(&itr,0);
    printf("value: %s index: %d \n",getIteratorValue(&itr),getIteratorIndex(&itr));
    printf("------ITERATOR TESTS COMPLETE-----\n");
}

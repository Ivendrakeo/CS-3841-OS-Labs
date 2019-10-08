#include <stdlib.h>
#include "llist.h"
#include "string.h"
#include <stdio.h>

void llInit(list *myList){
	myList->head = NULL;
	myList->tail = NULL;
}

int llSize(list *myList){
	int size = 0;
	node* workingNode = myList->head;
	while(workingNode != NULL){
		size++;
		workingNode = workingNode->next;
	}
	return size;
}

int llPushFront(list *myList,char *toStore){
	if(toStore != NULL){
		char *listString = malloc (strlen(toStore)+1);
		strcpy(listString, toStore);
		node* newNode = malloc (sizeof (node));
		newNode->prev = NULL;
		newNode->string = listString;
		newNode->next = myList->head;
		if(myList->head != NULL){
			myList->head->prev = newNode;
		}
		myList->head = newNode;
		if(myList->tail == NULL){
			myList->tail = newNode;
		}
		return 1;
	} else {
		return 0;
	}
}

int llPopFront(list *myList){
	if(myList->head != NULL){
		node* oldHead = myList->head;
		if(oldHead->next != NULL){
			myList->head = myList->head->next;
			myList->head->prev = NULL;	
		} else {
			myList->head = NULL;
			myList->tail = NULL;
		}
		free(oldHead->string);
		free(oldHead);
		return 1;
	} else {
		return 0;
	}
}

int llPushBack(list *myList, char *toStore){
	if(toStore != NULL){
		char *listString = malloc (strlen(toStore)+1);
		strcpy(listString, toStore);
		node* newNode = malloc (sizeof (node));
		newNode->prev = myList->tail;
		newNode->string = listString;
		newNode->next = NULL;
		if(myList->tail != NULL){
			myList->tail->next = newNode;
		}
		myList->tail = newNode;
		if(myList->head == NULL){
			myList->head = newNode;
		}
		return 1;
	} else {
		return 0;
	}
}

int llPopBack(list *myList){
	if(myList->tail != NULL){
		node* oldTail = myList->tail;
		if(oldTail->prev != NULL){
			myList->tail = myList->tail->prev;
			myList->tail->next = NULL;	
		} else {
			myList->tail = NULL;
			myList->head = NULL;
		}
		free(oldTail->string);
		free(oldTail);
		return 1;
	} else {
		return 0;
	}
}

void llClear(list *myList){
	int status = 1;
	while(status != 0){
		status = llPopBack(myList);
	}
}

int llInsertAfter(list* myList, node *insNode, char *toStore){
	if((insNode == NULL) || (toStore == NULL)){
		return 0;
	} else {
		char *listString = malloc (strlen(toStore)+1);
		strcpy(listString, toStore);
		node* newNode = malloc (sizeof (node));
		newNode->prev = insNode;
		newNode->string = listString;
		if(insNode == myList->tail){
			myList->tail->next = newNode;
			newNode->next = NULL;
			myList->tail = newNode;
		} else {
			newNode->next = insNode->next;
			insNode->next = newNode;
		}
		return 1;
	}
}

int llInsertBefore(list* myList, node *insNode, char *toStore){
	if((insNode == NULL) || (toStore == NULL)){
		return 0;
	} else {
		char *listString = malloc (strlen(toStore)+1);
		strcpy(listString, toStore);
		node* newNode = malloc (sizeof (node));
		newNode->next = insNode;
		newNode->string = listString;
		if(insNode == myList->head){
			myList->head->prev = newNode;
			newNode->prev = NULL;
			myList->head = newNode;
		} else {
			newNode->prev = insNode->prev;
			insNode->prev = newNode;
		}
		return 1;
	}
}

int llRemove(list* myList, node *rmvNode){
	node* workingNode = myList->head;
	if(!(rmvNode == NULL)){
		while(workingNode != NULL){
			if(workingNode == rmvNode){
				if((workingNode == myList->head) && (workingNode == myList->tail)){
					myList->head = NULL;
					myList->tail = NULL;
				} else if(workingNode == myList->head){
					workingNode->next->prev = NULL;
					myList->head = workingNode->next;
				} else if(workingNode == myList->tail){
					workingNode->prev->next = NULL;
					myList->tail = workingNode->prev;
				} else {
					workingNode->prev->next = workingNode->next;
					workingNode->next->prev = workingNode->prev;
				}
				free(workingNode->string);
				free(workingNode);
				break;
			} else {
				workingNode = workingNode->next;
			}
		}
		return 1;
	} else {
		return 0;
	}
}

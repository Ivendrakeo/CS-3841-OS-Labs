#include <stdlib.h>
#include "llist.h"
#include "string.h"
#include <stdio.h>
#include <pthread.h>

static void updateMaxSize(list* myList);

void llInit(list *myList){
	// Free these malloc's
	myList->mutex = malloc (sizeof(pthread_mutex_t));
	pthread_mutex_init((myList->mutex),NULL);
	myList->head = NULL;
	myList->tail = NULL;
	myList->size = 0;
	myList->maxSize = 0;
}

int llSize(list *myList){
	pthread_mutex_lock(myList->mutex);
	return myList->size;
	pthread_mutex_unlock(myList->mutex);
}

int llPushFront(list *myList,char *toStore){
	if(toStore != NULL){
		char *listString = malloc (strlen(toStore)+1);
		strcpy(listString, toStore);
		node* newNode = malloc (sizeof (node));
		newNode->prev = NULL;
		newNode->string = listString;
		pthread_mutex_lock(myList->mutex);
		newNode->next = myList->head;
		if(myList->head != NULL){
			myList->head->prev = newNode;
		}
		myList->head = newNode;
		if(myList->tail == NULL){
			myList->tail = newNode;
		}
		(myList->size)++;
		updateMaxSize(myList);
		pthread_mutex_unlock(myList->mutex);
		return 1;
	} else {
		return 0;
	}
}

int llPopFront(list *myList){
	if(myList->head != NULL){
		pthread_mutex_lock(myList->mutex);
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
		(myList->size)--;
		pthread_mutex_unlock(myList->mutex);
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
		newNode->string = listString;
		newNode->next = NULL;
		pthread_mutex_lock(myList->mutex);
		newNode->prev = myList->tail;
		if(myList->tail != NULL){
			myList->tail->next = newNode;
		}
		myList->tail = newNode;
		if(myList->head == NULL){
			myList->head = newNode;
		}
		(myList->size)++;
		updateMaxSize(myList);
		pthread_mutex_unlock(myList->mutex);
		return 1;
	} else {
		return 0;
	}
}

int llPopBack(list *myList){
	if(myList->tail != NULL){
		pthread_mutex_lock(myList->mutex);
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
		(myList->size)--;
		pthread_mutex_unlock(myList->mutex);
		return 1;
	} else {
		return 0;
	}
}

void llClear(list *myList){
	if(myList->size != 0){
		int status = 1;
		while(status != 0){
			status = llPopBack(myList);
		}
		myList->size = 0;
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
		pthread_mutex_lock(myList->mutex);
		if(insNode == myList->tail){
			myList->tail->next = newNode;
			newNode->next = NULL;
			myList->tail = newNode;
		} else {
			newNode->next = insNode->next;
			insNode->next = newNode;
		}
		(myList->size)++;
		updateMaxSize(myList);
		pthread_mutex_unlock(myList->mutex);
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
		pthread_mutex_lock(myList->mutex);
		if(insNode == myList->head){
			myList->head->prev = newNode;
			newNode->prev = NULL;
			myList->head = newNode;
		} else {
			newNode->prev = insNode->prev;
			insNode->prev = newNode;
		}
		(myList->size)++;
		pthread_mutex_unlock(myList->mutex);
		return 1;
	}
}

int llRemove(list* myList, node *rmvNode){
	if(!(rmvNode == NULL)){
		pthread_mutex_lock(myList->mutex);
		node* workingNode = myList->head;
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
		(myList->size)--;
		pthread_mutex_unlock(myList->mutex);
		return 1;
	} else {
		return 0;
	}
}

void printList(list* myList){
	int size = 0;
	printf("Printing List...\n");
	pthread_mutex_lock(myList->mutex);
	node* workingNode = myList->head;
	while(workingNode != NULL){
		printf("node: %d val: %s\n",size,workingNode->string);
		size++;
		workingNode = workingNode->next;
	}
	pthread_mutex_unlock(myList->mutex);
}

void printHeadAndTail(list* myList){
	pthread_mutex_lock(myList->mutex);
	if(!((myList->head == NULL) || (myList->tail == NULL)))
		printf("head: %s, tail: %s\n",myList->head->string,myList->tail->string);
	pthread_mutex_unlock(myList->mutex);
}

node* getNode(list* myList, int position){
	pthread_mutex_lock(myList->mutex);
	node* nodeWalker = myList->head;
	for(int i =0;i<position;i++){
		nodeWalker = nodeWalker->next;
	}
	pthread_mutex_unlock(myList->mutex);
	return nodeWalker;
}

void destructList(list* myList){
	free(myList->mutex);
}

/* Private helper method for keeping track of the list max size
 * 
 * myList : list which is being updated
 * 
 * return : void
 */
static void updateMaxSize(list* myList){
	if(myList->size > myList->maxSize){
		myList->maxSize = myList->size;
		}
}

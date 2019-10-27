/*
 * Introduction:
 * In this lab we simulated the operations of a burger place. threads were created for
 * each cook and customer where cooks placed burgers and fries on the warming rack and
 * customers take burgers and fries off the warming rack. Once all the cooks are done
 * they go home and signal to the customers the day is over. Finally, the statistics 
 * for the day will be printed at the end of the day.
 * 
 * Design:
 * For this lab I think the key was to break it into parts that were manageable such 
 * that the entire lab seemed hard but in smaller pieces it wasn't as bad. I then started
 * with updating my list the be "thread safe". From there, I created stubs for my thread
 * routines and layed out my main method for thread initalization, creation and join.
 * I created the thread routines and ensured they were working and returned. From there
 * I added support for statistic calculations and finished wrapping up the lab.
 * 
 * Build Instructions:
 * first $make then $./test workday.txt (and/or instead of workday.txt whichever
 * contains information for setting up the burger place.)
 * 
 * Analysis:
 *  - Is your implementation fair? 
 * 		My implementation is close to being "fair" assuming mutex'es are strong. When
 * 		Customers are done and need to get back "in line" if the mutex is strong will 
 * 		wait for all the customers in from before being unblocked. And the way I have it
 * 		implemented is by serving only one customer at a time.
 * 	-what is "fair"?
 * 		fairness refers to how often each thread is being allowed to run in it's critical
 * 		section.
 * 	-  In this implementation, when a customer arrives at the front of the line, if 
 * 	   there is not enough burgers and fries available, they hold up the entire line 
 * 	   and wait until their food is done.  What if instead, when a customer reached 
 * 	   the front of the line if not enough food was ready, they skip their ordering 
 * 	   opportunity and immediately return to the end of the line without ordering?  
 * 	   Given your definition of fair, is this behavior fair?  Explain why or why not?
 *     Yes. The customer holds up the entire line blocking until their order is filled.
 * 	   If their order could not be filled and it would be available to the next customer
 * 	   I would suggest that this is not fair because depending on how fast the burgers and
 * 	   fries become available customers with smaller orders could get back in line and get
 * 	   their order filled again before ever letting the first customers order get filled.
 * 	- How could orders be prepared instead?
 * 		orders / customers could be added to a list queue and there would be an observing
 * 		thread which monitors the queue and when enough burgers / fires are on the list 
 * 		for the next order to be filled, it gets popped and filled.
 * 	- How could we modify for warmer capacity limits?
 * 		Have cooks / friers check the warmer before cooking and if capacity was reached
 * 		then wait until the they have been used.
 * 	- Conclustion:
 * 		The most challenging part of this lab for me was debugging a memory leak that I never
 * 		found... Other then that, I spent a fair amount of time ensuring my mutex'es were 
 *		being properly locked and unlocked when I wanted. I liked the concept for the lab 
 * 		because the thread routines were actually not that complicated and I wouldn't know
 * 		where to even begin.
 */

#include <stdlib.h>
#include <stdio.h>
#include "llist.h"
#include <unistd.h>
#include <pthread.h>
#include "BurgerPlace.h"

// Top level mutex for managing interactions between cooks and customers
static pthread_mutex_t listMutex;
static pthread_mutex_t orderMutex;

// Flag to signal to the customers that the day is over after all the cooks have finished.
static int dayOverFlag;

// Shared burger and fries queue's 
static list warmingBurgers;
static list warmingFries;

/*
 * The burgerCookThread method will be the entry point for burger cook threads.
 * Will add a burger to the warmingBurgers list every timing point defined by the BurgerCooks 
 * sruct passed in.
 *
 * cook : pointer to BurgerCooks strct containing initalization information for the number of
 *        burgers the cook cooks and duration.
 *
 * return : void* containing nothing.
 */
static void* burgerCookThread(void* cook);

/*
 * The fryerThread method will be the entry point for fryer threads.
 * Will add an order of fries to the warmingFries list every timing point defined by the Fryers 
 * sruct passed in.
 *
 * cook : pointer to Fryers strct containing initalization information for the number of
 *        orders of fires the cook cooks and duration.
 *
 * return : void* containing nothing.
 */
static void* fryerThread(void* fryer);

/*
 * The customerThread method will be the entry point for customer threads.
 * Will take a number of burgers and fries every timing point defined by the passed in
 * Customer struct
 *
 * customer : Customer struct containing the number of burgers and fries they will take 
 *            and the rate at which they take their order. Also a counter that keeps track
 *            of the number of orders they get filled.
 *
 * return : returns after the dayOverFlag is set to 1
 */
static void* customerThread(void* customer);

static void checkToExit();

// Main method
int main(int argc, char* argv[]){

	printf("Initializing and seting up threads...\n");
	// initialize burger and fries list
	llInit(&warmingBurgers);
	llInit(&warmingFries);
	pthread_mutex_init(&listMutex,NULL);
	pthread_mutex_init(&orderMutex,NULL);
	dayOverFlag = 0;

	// create and fill burger cooks, fryers, and customer structures with data from file
	BurgerCooks burger_cooks;
	Fryers fryers;
	CustomerArray* customer_array = initBurgerPlace(argv[1], &burger_cooks, &fryers);

	//initalize thread counts for all fryers, cooks, and customers
	pthread_t cook_threads[burger_cooks.number_cooks];
	pthread_t fryer_threads[fryers.number_cooks];
	pthread_t customer_threads[customer_array->num_customers];

	printf("Starting threads!\n");
	// create burger cook threads
	for(int i = 0; i < burger_cooks.number_cooks; ++i)
		pthread_create(&cook_threads[i], NULL, &burgerCookThread, (void *)(&burger_cooks));
	// create fryer threads
	for(int i = 0; i < fryers.number_cooks; ++i)
		pthread_create(&fryer_threads[i], NULL, &fryerThread, (void *)(&fryers));
	// create customer threads
	for(int i = 0; i < customer_array->num_customers; ++i)
		pthread_create(&customer_threads[i], NULL, &customerThread, (void *)(customer_array->customers + i));
	
	printf("Waiting for threads to finish.\n");
	// wait for the cooks and friers to all go home
	for(int i = 0; i < burger_cooks.number_cooks; ++i)
		pthread_join(cook_threads[i], NULL);
	for(int i = 0; i < fryers.number_cooks; ++i)
		pthread_join(fryer_threads[i], NULL);
	
	printf("Day is over, time for customers to go home.\n");
	// stop all the customers
	dayOverFlag = 1;
	for(int i = 0; i < customer_array->num_customers; ++i)
		pthread_join(customer_threads[i], NULL);

	// print results
	printf("Success! Calculating results...\n");
	printf("Max # Burgers %d\n",warmingBurgers.maxSize);
	printf("Max # Fries %d\n",warmingFries.maxSize);
	for(int i = 0; i < customer_array->num_customers; ++i)
		printf("Customer %d had %d orders filled\n", i, customer_array->customers[i].ordersFilled);

	// cleanup memory
	printf("Cleaning up resources\n");
	deleteCustomers(customer_array);
	llClear(&warmingBurgers);
	destructList(&warmingBurgers);
	llClear(&warmingFries);
	destructList(&warmingFries);

	return 0;
}

static void* burgerCookThread(void* cook){
	BurgerCooks burger_cook = *((BurgerCooks *) cook);
	for(int i = 0; i < burger_cook.total_servings; ++i){
		usleep(burger_cook.cook_time);
		pthread_mutex_lock(&listMutex);
		llPushFront(&warmingBurgers, "-");
		pthread_mutex_unlock(&listMutex);
	}
	pthread_exit(0);
}

static void* fryerThread(void* fryer){
	Fryers fryer_cook = *((Fryers *) fryer);
	for(int i = 0; i < fryer_cook.total_servings; ++i){
		usleep(fryer_cook.cook_time);
		pthread_mutex_lock(&listMutex);
		llPushFront(&warmingFries, "-");
		pthread_mutex_unlock(&listMutex);
	}
	pthread_exit(0);
}

static void* customerThread(void* customer){
	Customer customer_n = *((Customer *) customer);
	while(1){
		int flag = 0;
		pthread_mutex_lock(&orderMutex);
		for(int i = 0; i < customer_n.burgers; ++i){
			while(!flag){
				pthread_mutex_lock(&listMutex);
				flag = llPopFront(&warmingBurgers);
				pthread_mutex_unlock(&listMutex);
				checkToExit();
			}
			flag = 0;
		}
		for(int i = 0; i < customer_n.fries; ++i){
			while(!flag){
				pthread_mutex_lock(&listMutex);
				flag = llPopFront(&warmingFries);
				pthread_mutex_unlock(&listMutex);
				checkToExit();
			}
			flag = 0;
		}
		pthread_mutex_unlock(&orderMutex);
		((Customer *) customer)->ordersFilled = ((Customer *) customer)->ordersFilled + 1;
		usleep(customer_n.duration);
	}
	return NULL;
}

static void checkToExit(){
	if(dayOverFlag == 1){
		pthread_mutex_unlock(&orderMutex);
		pthread_exit(0);
	}
}
#include <stdlib.h>
#include <stdio.h>
#include "llist.h"
#include <unistd.h>
#include <pthread.h>
#include "BurgerPlace.h"

// Top level mutex for managing interactions between cooks and customers
static pthread_mutex_t listMutex;

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
		((Customer *) customer)->ordersFilled = ((Customer *) customer)->ordersFilled + 1;
		usleep(customer_n.duration);
	}
	return NULL;
}

static void checkToExit(){
	if(dayOverFlag == 1)
		pthread_exit(0);
}
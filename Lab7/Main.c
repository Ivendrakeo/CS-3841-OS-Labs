#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "llist.h"
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "ConstructionTraffic.h"

// semaphore to manage the max number of cars in the traffic circle
sem_t traffic_sem;

// flag to tell flagger that all the cars are done and to end
int flagger_flag;

// int array to for flagger to signal to a car that it can go though the traffic circle
// a '1' in their car's position will signal to that car that it can go though the traffic circle.
int* car_signals;

// public array of all cars
CarArray* car_array;

// queues for each cars waiting on the west and east. 
static list westCarQueue;
static list eastCarQueue;

static void* flaggerThread(void* flagger);

static void* carThread(void* car);


// Main method
int main(int argc, char* argv[]){

	printf("Initializing and seting up threads...\n");
	// initialize car lists
	llInit(&westCarQueue);
	llInit(&eastCarQueue);
	flagger_flag = 0;

	// create and fill car and flagger structs from file
	Flagger flagger;
	car_array = initTrafficCircle(argv[1], &flagger);

	// setup array for IPC communication between flagger and car threads
	car_signals = malloc(sizeof(int) * car_array->num_cars);
	for(int i = 0; i < car_array->num_cars; ++i){
		// initialize car signals to zero's
		*(car_signals + i) = 0;
		// initialize cars in their respective initial queues
		car_array->cars[i].initial_side ? llPushBack(&eastCarQueue, i) : llPushBack(&westCarQueue, i);
	}

	//setup traffic semiphore to allow the max number of cars in the traffic circle
	sem_init(&traffic_sem, 0, flagger.car_capacity);

	//initalize thread counts for the flagger and each car
	pthread_t flagger_thread;
	pthread_t car_threads[car_array->num_cars];

	printf("Starting threads!\n");
	// create flagger thread
	pthread_create(&flagger_thread, NULL, &flaggerThread, (void *)(&flagger));
	// create car threads
	for(int i = 0; i < car_array->num_cars; ++i)
		pthread_create(&car_threads[i], NULL, &carThread, (void *)(&(car_array->cars[i])));

	printf("Waiting for threads to finish.\n");
	// wait for all cars to be done
	for(int i = 0; i < car_array->num_cars; ++i)
		pthread_join(car_threads[i], NULL);
	
	printf("Day is over, time for flagger to go home.\n");
	// stop flagger
	flagger_flag = 1;
	pthread_join(flagger_thread, NULL);

	// print results

	// cleanup memory
	printf("Cleaning up resources\n");
	deleteCars(car_array);
	llClear(&westCarQueue);
	destructList(&westCarQueue);
	llClear(&eastCarQueue);
	destructList(&eastCarQueue);

	return 0;
}

static void* flaggerThread(void* flagger){
	Flagger man = *((Flagger *) flagger);
	while(1){
		// ensure all cars are out of the traffic circle
		int flag = 0;
		do{
			flag = 0;
			for(int i = 0; i < car_array->num_cars; ++i)
				if(car_signals[i]){
					flag = 1;
				}
		}while(flag);
		int rotation = clock() * 1000000 / CLOCKS_PER_SEC; // start timer
		int end_rotation = rotation + man.flow_time; // needed time (in microseconds)
		// allow traffic to flow in direction for designated time
		int sem_val = 0;
		do {
			// check for day over
			if(flagger_flag){
				pthread_exit(EXIT_SUCCESS);
			}
			//if there is a spot in the traffic circle, let the next car though and signal to the car to start going through
			sem_getvalue(&traffic_sem, &sem_val);
			if(sem_val >= 0){
				sem_wait(&traffic_sem);
				int popped_car = man.current_direction ? llPopFront(&eastCarQueue) : llPopFront(&westCarQueue);
				car_signals[popped_car] = 1;
			}
			rotation = clock() * 1000000 / CLOCKS_PER_SEC;
		} while (rotation <= end_rotation);
		man.current_direction = !man.current_direction; // flip traffic direction
	}
}

static void* carThread(void* car){
	Car driver = *((Car *) car);
	int total_crossings = 0;
	int side = driver.initial_side;
	while(total_crossings < driver.num_crossings){
		int signal = 0;
		// wait for flagger to tell the car it can go
		while(!signal)
			signal = car_signals[driver.car_number];
		// pass though intersection
		usleep(driver.drive_time);
		// don't add car to queue if it's on it's last run
		if(total_crossings < driver.num_crossings - 1){
			// add car to other queue
			side = !side;
			side ? llPushBack(&eastCarQueue, driver.car_number) : llPushBack(&westCarQueue, driver.car_number);
		}
		// tell flagger car is no longer in intersection
		car_signals[driver.car_number] = 0;
		total_crossings = total_crossings + 1;
	}
	pthread_exit(EXIT_SUCCESS);
}
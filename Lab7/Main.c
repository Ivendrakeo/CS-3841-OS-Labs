/*
 * Design:
 * The way I attacked this lab was by creating a master flagger thread which manages signaling and queuing of cars in the traffic circle. 
 * The flagger would start by making sure the site was cleared which involved checking each car's global boolean varriable which
 * monitored if it was in the circle or not. Once it determined there was no cars in the circle, it started a loop timer that was the 
 * duration before changing direction. In the loop it would check if the site was full of cars and if not, it would pop a car off the 
 * queue and signal to that car though a condition varriable that it should start going through the site. The flagger would keep 
 * repeating this cycle until all the cars finished.
 * In the car thread it would track waiting duration and number of completions locally while waiting for the condition varriable
 * to be signaled. Once signaled it would sleep for the required duration and signal back to the flagger through global memory.
 * 
 * Build Instructions: $make ./test day.txt
 * 
 * Analysis: 
 * I believe my implementation to be as fair as possible. Implementing the feedback from the previous burger lab, I decided to
 * create the extra overhead for car queue's on each (east and west) sides. This ensures cars are being popped in the order that
 * they should (and as fairly as possible). When evaluating the actual time they waited, I feel like this is where I fall since
 * there is unnessicary waiting time from the car signaling back to the flagger because the global memory flags were not as fast
 * as I would expected. But comparing the wait times of each car, it's clear that they are all waiting similar lengths, so I 
 * guess that part it "fair".
 * If you wanted to implement a priority queue which let faster cars though first, this could be done by modifying the list such 
 * that as cars were pushed into the queue, the list it's self could inset the car into the position based on it's travel time.
 * This would guarntee when popping that the fastest cars go first. Unfortunatly, this may cause starvation for the slower cars
 * if they don't get to go though on that rotation...
 * 
 * Conclusion:
 * I liked the challenge of this lab. I feel that it was sufficiently different then the burger lab even though on the surface
 * it initially seemed similar. It was for sure a fun challenge to get one thread to manage others.
 * The real problem in this lab lies in it's timing. Or at least, I tried many strategies to get the signaling of the cars and 
 * signaling back to be quick so that it wouldn't be that noticable on the front of the direction changing or the car wait time.
 * Unfortunatly, with the test case of 200us for the direction flipping, it was being impacted heavily by these outside factors.
 * Even the time to loop around and check to see if another car could go though the site was ~30us for my implementation which 
 * is extremely significant and surly impacted the results.
 * 
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "llist.h"
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "ConstructionTraffic.h"
#include <signal.h>

// Mutex which manages top level interactions witch the car queue access.
pthread_mutex_t list_mutex;

// semaphore to manage the max number of cars in the traffic circle
int traffic_sem;

// flag to tell flagger that all the cars are done and to end
int flagger_flag;

// int array to for flagger to signal to a car that it can go though the traffic circle
// a '1' in their car's position will signal to that car that it can go though the traffic circle.
int* car_signals;
Signal* mutex_signals;

// public array of all cars
CarArray* car_array;

// queues for each cars waiting on the west and east. 
static list westCarQueue;
static list eastCarQueue;

// Thread which run's the flagger
static void* flaggerThread(void* flagger);

// Threads which run each car
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
	mutex_signals = malloc(sizeof(Signal) * car_array->num_cars);
	for(int i = 0; i < car_array->num_cars; ++i){
		// initialize car signals to zero's
		*(car_signals + i) = 0;
		//sem_init(&car_signals[i], 0, 0);
		pthread_mutex_init(&(mutex_signals[i].m), NULL);
		pthread_cond_init(&(mutex_signals[i].cv), NULL);
		// initialize cars in their respective initial queues
		car_array->cars[i].initial_side ? llPushBack(&eastCarQueue, i) : llPushBack(&westCarQueue, i);
	}

	// setup traffic semiphore to allow the max number of cars in the traffic circle
	//sem_init(&traffic_sem, 0, flagger.car_capacity);
	traffic_sem = 0;
	pthread_mutex_init(&list_mutex, NULL);

	// initalize thread counts for the flagger and each car
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
	for(int i = 0; i < car_array->num_cars; ++i)
		printf("Car %d waited a total of %d ns\n", car_array->cars[i].car_number, car_array->cars[i].drive_time);

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
	//printf("flagger cap: %d, dir %d, flow %d\n",man.car_capacity, man.current_direction, man.flow_time);
	while(1){
		// ensure all cars are out of the traffic circle
		int flag = 0;
		do{
			flag = 0;
			for(int i = 0; i < car_array->num_cars; ++i){
				//int val = 0;
				//sem_getvalue(&car_signals[i], &val);
				//if(val)
				//	flag = 1;
				if(car_signals[i]){
					flag = 1;
				}
			}
		}while(flag);
		//printf("all cars are cleared from intersection\n");
		long rotation = clock() * 1000000 / CLOCKS_PER_SEC; // start timer
		long end_rotation = rotation + man.flow_time; // needed time (in microseconds)
		// allow traffic to flow in direction for designated time
		do {
			// check for day over
			if(flagger_flag){
				pthread_exit(EXIT_SUCCESS);
			}
			//if there is a spot in the traffic circle, let the next car though and signal to the car to start going through
			//int sem_val = 0;
			//sem_getvalue(&traffic_sem, &sem_val);
			if(traffic_sem < man.car_capacity){
				pthread_mutex_lock(&list_mutex);
				int popped_car = man.current_direction ? llPopFront(&eastCarQueue) : llPopFront(&westCarQueue);
				pthread_mutex_unlock(&list_mutex);
				if(popped_car != -1){
					car_signals[popped_car] = 1;
					//sem_post(&car_signals[popped_car]);
					traffic_sem = traffic_sem + 1;
					//sem_wait(&traffic_sem);
					pthread_cond_signal(&mutex_signals[popped_car].cv);
					//printf("signaling to car %d w/ %d cars in circle\n", popped_car, traffic_sem);
					//printf("flagger clock: %d\n", clock() * 1000000 / CLOCKS_PER_SEC);
				}
			}
			rotation = clock() * 1000000 / CLOCKS_PER_SEC;
		} while (rotation <= end_rotation);
		man.current_direction = !man.current_direction; // flip traffic direction
		printf("Flagger indicating safe to drive %s \n", (man.current_direction == 1 ? "EAST->WEST" : "WEST->EAST"));
	}
}

static void* carThread(void* car){
	Car driver = *((Car *) car);
	//printf("car %d created\n", driver.car_number);
	int total_crossings = 0;
	int side = driver.initial_side;
	while(total_crossings < driver.num_crossings){
		//int signal = 0;
		// wait for flagger to tell the car it can go
		int clock_start = clock() * 1000000 / CLOCKS_PER_SEC;
		//while(!signal)
		//	signal = car_signals[driver.car_number];
		//int val = 0;
		//sem_getvalue(&car_signals[driver.car_number], &val);
		//while(!val)
		//	sem_getvalue(&car_signals[driver.car_number], &val);
		pthread_mutex_lock(&mutex_signals[driver.car_number].m);
		//int ret = 0;
		//do{
		pthread_cond_wait(&mutex_signals[driver.car_number].cv, &mutex_signals[driver.car_number].m);
		//}while(!ret);
		((Car *) car)->drive_time = ((Car *) car)->drive_time + ((clock() * 1000000 / CLOCKS_PER_SEC) - clock_start);
		// pass though intersection
		printf("Car %d entering construction zone traveling: %s Crossings remaining: %d \n",
				driver.car_number, (side == 0 ? "EAST->WEST" : "WEST->EAST"),driver.num_crossings - total_crossings);
		//printf("car %d signaled with %d. sleepint %d\n", driver.car_number, car_signals[driver.car_number], driver.sleep_time);
		//printf("car clock: %d\n", clock() * 1000000 / CLOCKS_PER_SEC);
		usleep(driver.sleep_time);
		// don't add car to queue if it's on it's last run
		if(total_crossings < driver.num_crossings - 1){
			// add car to other queue
			side = !side;
			pthread_mutex_lock(&list_mutex);
			side ? llPushBack(&eastCarQueue, driver.car_number) : llPushBack(&westCarQueue, driver.car_number);
			pthread_mutex_unlock(&list_mutex);
		}
		// tell flagger car is no longer in intersection
		//printf("car %d leaving intersection\n", driver.car_number);
		car_signals[driver.car_number] = 0;
		//sem_wait(&car_signals[driver.car_number]);
		total_crossings = total_crossings + 1;
		traffic_sem = traffic_sem - 1;
		//sem_post(&traffic_sem);
		pthread_mutex_unlock(&mutex_signals[driver.car_number].m);
	}
	//printf("exiting car thread %d\n", driver.car_number);
	pthread_exit(EXIT_SUCCESS);
}
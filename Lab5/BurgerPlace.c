// standard libraries
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h> // for threading
#include <sys/types.h> // for wait, etc.
#include <sys/wait.h> // for wait, etc.
#include <unistd.h> // fork, sleep, etc.

// user defined
#include "llist.h"
#include "string.h"
#include "BurgerPlace.h"

CustomerArray* initBurgerPlace(const char* filename, BurgerCooks* burger_cooks, Fryers* fryers){
	FILE* input = fopen(filename, "r");
	// start reading the file
	if(input != NULL){
        // fill the burger_cooks and fryers
		fscanf(input, "%d", &(burger_cooks->number_cooks));
		fscanf(input, "%d", &(burger_cooks->cook_time));
        fscanf(input, "%d", &(burger_cooks->total_servings));
        fscanf(input, "%d", &(fryers->number_cooks));
		fscanf(input, "%d", &(fryers->cook_time));
        fscanf(input, "%d", &(fryers->total_servings));
        // create and fill the customer array with each following customer
        CustomerArray* customer_array = malloc (sizeof(CustomerArray));
        fscanf(input, "%d", &(customer_array->num_customers));
        customer_array->customers = malloc (sizeof(Customer)*customer_array->num_customers);
        for(int i = 0; i < customer_array->num_customers; ++i){
            fscanf(input, "%d", &((customer_array->customers + i)->burgers));
            fscanf(input, "%d", &((customer_array->customers + i)->fries));
            fscanf(input, "%d", &((customer_array->customers + i)->duration));
            (customer_array->customers + i)->ordersFilled = 0;
        }
		//close the file and return the resulting customer array
		fclose(input);
		return customer_array;
	}
	return NULL;
}

void deleteCustomers(CustomerArray* customer_array){
    free(customer_array->customers);
    free(customer_array);
}

// standard libraries
#include <stdlib.h>
#include <stdio.h>

// user defined
#include "llist.h"
#include "string.h"
#include "ConstructionTraffic.h"

CarArray* initTrafficCircle(const char* filename, Flagger* flagger){
	FILE* input = fopen(filename, "r");
	// start reading the file
	if(input != NULL){
        int west_cars, east_cars, drive_time = 0;
		fscanf(input, "%d", &west_cars);
        fscanf(input, "%d", &east_cars);
        fscanf(input, "%d", &drive_time);
		fscanf(input, "%d", &(flagger->flow_time));
        fscanf(input, "%d", &(flagger->car_capacity));
        flagger->current_direction = 0;
        // create and fill the car array
        CarArray* car_array = malloc (sizeof(CarArray));
        car_array->num_cars = (west_cars + east_cars);
        car_array->cars = malloc(sizeof(Car) * car_array->num_cars);
        for(int i = 0; i < car_array->num_cars; ++i){
            fscanf(input, "%d", &((car_array->cars + i)->num_crossings));
            fscanf(input, "%d", &((car_array->cars + i)->sleep_time));
            (car_array->cars + i)->car_number = i;
            (car_array->cars + i)->drive_time = 0;
            if(i < west_cars){
                (car_array->cars + i)->initial_side = 0;
            } else {
                (car_array->cars + i)->initial_side = 1;
            }
        }
		//close the file and return the resulting customer array
		fclose(input);
		return car_array;
	}
	return NULL;
}

void deleteCars(CarArray* car_array){
    free(car_array->cars);
    free(car_array);
}

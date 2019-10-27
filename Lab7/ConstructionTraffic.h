#ifndef CONTRA_H
#define CONTRA_H

typedef struct Flagger{
    int flow_time;
    int car_capacity;
    int current_direction;
} Flagger;

/*
 *
 * initial_side : boolean value. 0 if west, 1 if east.
 */
typedef struct Car{
    int drive_time;
    int initial_side;
    int num_crossings;
    int sleep_time;
    int car_number;
} Car;

typedef struct CarArray{
    Car* cars;
    int num_cars;
} CarArray;

CarArray* initTrafficCircle(const char* filename, Flagger* flagger);

void deleteCars(CarArray* car_array);

#endif
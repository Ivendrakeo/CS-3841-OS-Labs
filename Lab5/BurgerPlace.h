#ifndef BURGERPLACE_H
#define BURGERPLACE_H


/*
 * Struct which maintains all of the initial information about the burger cooks when setting
 * up the burger place.
 *
 * number_cooks : total number of burger cooks upon the start of the day
 * cook_time : total time it takes 1 burger cook to create 1 burger
 * total_servings : number of burgers each cook creates before they leave for the day
 */
typedef struct BurgerCooks{
    int number_cooks;
    int cook_time;
    int total_servings;
}BurgerCooks;

/*
 * Struct which maintains all of the initial information about the Fryers when setting
 * up the burger place.
 *
 * number_cooks : total number of fry cooks upon the start of the day
 * cook_time : total time it takes 1 fryer to create 1 order of fries
 * total_servings : number of orders of fries each cook creates before they leave for the day
 */
typedef struct Fryers{
    int number_cooks;
    int cook_time;
    int total_servings;
}Fryers;

/*
 * Struct which maintains information about a single customer at the burger place.
 *
 * burgers : number of burgers the customer will buy when they reach the counter
 * fries : number of fries the customer will buy when they reach the counter
 * duration : amount of time the customer waits before returning to the counter
 * ordersFilled : amount of completed customer orders (to be read by parent)
 */
typedef struct Customer{
    int burgers;
    int fries;
    int duration;
    int ordersFilled;
}Customer;

/*
 * Struct which maintains a listing of all customers at the burger place. 
 *
 * num_customers : total number of customers at the burger place.
 * customers : array of length num_customers containing type Customer data for each 
 *             for each customer in the burger place
 */
typedef struct CustomerArray{
    int num_customers;
    Customer* customers;
}CustomerArray;

/*
 * Initalizes the burger place by reading the input file and filling out the BurgerCooks, 
 * Fryers, and creating a CustomerArray
 *
 * filename : file name or location which contains the burgerplace parsable information
 * burger_cooks : empty BurgerCooks struct to be populated by the file data
 * fryers : empty Fryers struct to be populated by the file data
 * customer_array : empty CustomerArray struct to be populated by the file data
 * 
 * return : CustomerArray* which contains the number of customers and each customers data
 */
CustomerArray* initBurgerPlace(const char* filename, BurgerCooks* burger_cooks, Fryers* fryers);

/*
 * Deletes the customers by free'ing all of the customer array data 
 *
 * customer_array : CustomerArray strcut to be free'd
 * 
 * return : void, but free'd struct data for the customer array
 */
void deleteCustomers(CustomerArray* customer_array);

#endif

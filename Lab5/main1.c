// This is the main for the lab 5 in os

// Include Libraries
#include <stdio.h>
#include <stdlib.h> // For atoi function
#include "headerArray.h"
#include <pthread.h>

pthread_mutex_t lock; // For the mutex lock

int main(int argc, char *argv[]){

// Take in the values from the command line
if((argc != 4)||((atoi(argv[1])==0)&&(atoi(argv[2])==0)&&(atoi(argv[3])==0))){

    printf("please know how to use this program!");
    return 1;
}
else{

// Convert arguments from strings to integers
    int resource1 = atoi(argv[1]);
    int resource2 = atoi(argv[2]);
    int resource3 = atoi(argv[3]);  

// This will fill the array holding the total system resources for the system
fillMaxResources(resource1, resource2, resource3);
printf("Here are the values for the total resources: %d, %d, %d\n", resource1, resource2, resource3);
}
// maxSystemResources[][], is now filled

// Now we fill the max need array for each of the customers
fillMaximumNeed();
// The maximum need array is now filled with acceptable values,
printf("\nHere is the randomly generated maximum need array\n");
print2DArray(maximumNeed);
printf("\n");
// Maximum need array is now filled

// Display the arrays
printf("\nHere is the available resources array: \n");
initializeAvailableResources();
printAvailable();
printf("\n");
printf("Here is the currently allocated array: \n");
print2DArray(allocatedCurrently);
printf("\n");

// Now, resources will try to be requested from the system
printf("\nNow, we are now going to try and request resources from the system\n");
serviceCustomers();
printf("\n\nBellow is the maxium need array\n");
print2DArray(maximumNeed);
// The above code is for testing the functionalities for the algorithms
printf("\n\n\nThe above represents the current system state, and replicates a natural system in use\n\n\n");

printf("\n\n Now lets get our threads created and get them to randomly request and deallocate resources\n\n");

// The following code is for threads

pthread_t threadIDs[NUMBER_OF_CUSTOMERS];
int threadArgs[NUMBER_OF_CUSTOMERS];
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
        threadArgs[i] = i + 1; // Assign thread argument numbers 1 through 5
        if (pthread_create(&threadIDs[i], NULL, threadFunction, (void *)&threadArgs[i]) != 0) {
            perror("Failed to create thread");
            return EXIT_FAILURE;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
        pthread_join(threadIDs[i], NULL);
    }

    printf("\nWe made it to the end\n");

return 0;

}

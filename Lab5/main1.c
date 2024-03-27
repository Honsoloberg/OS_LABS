// This is the main for the lab 5 in os

// Include Libraries
#include <stdio.h>
#include <stdlib.h> // For atoi function
#include "headerArray.h"

int main(int argc, char *argv[]){

// Take in the values from the command line
if(argc != 4){

    printf("please know how to use this program!");
    return 1;
}else{

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
printf("\nHere is the maximum need array\n");
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

return 0;

}
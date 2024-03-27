#ifndef headerArray
#define headerArray

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

#include <stdbool.h> // For bool, true, false
#include <time.h>

// maxSystemResources array holds the amount of total resources the system has
int maxSystemResources[NUMBER_OF_RESOURCES];

// Holds the currently available (not allocated) resources of the system
int available[NUMBER_OF_RESOURCES];

// Holds the maximum need of each of the customers 
int maximumNeed[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Holds the remaining need for each of the customers
int needRemaining[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Holds the currently allocated resources for the customers
int allocatedCurrently[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Hold the temporary data used for requests
int tempRow[NUMBER_OF_RESOURCES];

// (working)
// Function used at the start of the program to define the total available resouces for the system
void fillMaxResources(int num1, int num2, int num3){
    maxSystemResources[0] = num1;
    maxSystemResources[1] = num2;
    maxSystemResources[2] = num3;
}

// This method will print out the contents of the array
void printMaxResources() {
    for(int i = 0; i < 3; i++) {
        printf("%d ", maxSystemResources[i]);
    }
    printf("\n"); // Print a newline for better output formatting
}

// This method will try to request resouces from the system
int request_Resources(int customer_Num, int request[]) {
    // Step 1: Check if Requesti ≤ Needi
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > needRemaining[customer_Num][i]) {
            printf("Error: Customer %d has exceeded its maximum claim.\n", customer_Num);
            return -1; // Error: Process has exceeded its maximum claim
        }
    }

    // Step 1.a: Check if Requesti ≤ Availablei
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            // Step 1.a.i: Process must wait, as resources are not available
            printf("Customer %d must wait, as resources are not available.\n", customer_Num);
            return -1; // Resources are not available
        }
    }

    // If the program reaches here, it means Requesti ≤ Needi and Requesti ≤ Availablei
    // Step 1.a.i: Available = Available - Requesti
    // Allocationi = Allocationi + Requesti
    // Needi = Needi - Requesti
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocatedCurrently[customer_Num][i] += request[i];
        needRemaining[customer_Num][i] -= request[i];
    }

    printf("Request for Customer %d has been granted.\n", customer_Num + 1);
    return 0; // Request has been granted
}


int release_Resources(int customer_Num, int release[]){
for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocatedCurrently[customer_Num][i] -= release[i];
        needRemaining[customer_Num][i] += release[i];
    }
return 0; // -1 if cant do, why should this ever be -1?
}

int are_We_Safe() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false}; // Finish[i] = false for all customers initially
    
    // Step 1: Initialize work with available resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    bool found = false;
    do {
        found = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) { // If Finish[i] == false
                bool enoughResources = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    // Step 2.a: Check if Needi ≤ Work for all resources
                    if (needRemaining[i][j] > work[j]) {
                        enoughResources = false;
                        break;
                    }
                }
                
                if (enoughResources) {
                    // If a process can complete, add its allocations to work and mark it as finished
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocatedCurrently[i][j];
                    }
                    finish[i] = true;
                    found = true; // A customer was able to complete, so we loop again to check remaining customers
                }
            }
        }
    } while (found); // If no customer was able to proceed in the last iteration, exit the loop
    
    // Step 3: Check if all customers are finished
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) {
            // Step 4: System is in an unsafe state
            printf("Unsafe state\n");
            return 0; // System is not safe
        }
    }
    
    // If we reach here, all customers can finish safely
    printf("Safe state\n");
    return 1; // System is safe
}

// Function for testing
void fillForTesting(){
    int arr[] = {1, 2, 3};
request_Resources(1, arr);
}

// Function to print the contents of a dynamically sized array
void print2DArray(int arr[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]) {
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for(int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n"); // New line after each row
    }
}

// Must add a check to see if max need is all 0's (FIX ME)
// This function will fill the max need array for each person that will not exceed the total resources for the system
void fillMaximumNeed() {
    srand(time(NULL));
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            // Generate a random number in the range [0, maxSystemResources[j]]
            maximumNeed[i][j] = rand() % (maxSystemResources[j] + 1);
        }
    }
}

// This will calculate the need remaining array
void calculateNeedRemaining() {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            needRemaining[i][j] = maximumNeed[i][j] - allocatedCurrently[i][j];
        }
    }
}


// Add a feature to this function to check wether or not the customer already has all of the resources they need
// If the customer already has all of the resources they need, snap the threads neck after waiting for a second (FIX ME)

// This function change the temp array used for resource requests
void generateSmallerRow(int rowIndex) {
    // Ensure the rowIndex is within bounds
    if (rowIndex < 0 || rowIndex >= NUMBER_OF_CUSTOMERS) {
        printf("Invalid row index.\n");
        return;
    }
    
    // Calculates the current remaining needs for the program
    calculateNeedRemaining();

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        // Generate a random number in the range [0, needRemaining[rowIndex][i]]
        tempRow[i] = rand() % (needRemaining[rowIndex][i] + 1);
    }
}

// Prints the available resources for the system
void printAvailable() {
    printf("Available Resources: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    printf("\n");
}

// Initializes the available resources for the system
void initializeAvailableResources() {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = maxSystemResources[i];
    }
}

//Used for testing, Prints the temp array
void printTempRow() {
    printf("tempRow[]: ");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", tempRow[i]);
    }
    printf("\n");
}

// Main loop for serving customers
void serviceCustomers() {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Servicing customer %d\n", i + 1); // +1 to start counting customers from 1
        generateSmallerRow(i);
        printf("This is the requesting resource temp array\n");
        printTempRow();
        printf("\n");
        // This displays the current request for the customer
        request_Resources(i,tempRow);
        printf("\nThis is the current allocated array\n");
        print2DArray(allocatedCurrently);

    }
}










#endif // headerArray

#ifndef headerArray
#define headerArray

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

#include <stdbool.h> // For bool, true, false
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// Holds the resources to be release for a certain customer
int resources_to_release[NUMBER_OF_RESOURCES];
pthread_mutex_t resourceReleaseLock = PTHREAD_MUTEX_INITIALIZER;

// maxSystemResources array holds the amount of total resources the system has
int maxSystemResources[NUMBER_OF_RESOURCES];

// Holds the currently available (not allocated) resources of the system
int available[NUMBER_OF_RESOURCES];
pthread_mutex_t availableLock = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for available

// Holds the maximum need of each of the customers 
int maximumNeed[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
pthread_mutex_t maximumNeedLock = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for maximumNeed

// Holds the remaining need for each of the customers
int needRemaining[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
pthread_mutex_t needRemainingLock = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for needRemaining

// Holds the currently allocated resources for the customers
int allocatedCurrently[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
pthread_mutex_t allocatedCurrentlyLock = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for allocatedCurrently

// Hold the temporary data used for requests
int tempRow[NUMBER_OF_RESOURCES];
pthread_mutex_t tempRowLock = PTHREAD_MUTEX_INITIALIZER; // Mutex lock for tempRow

// Declare a mutex for resource allocation synchronization (not working fully yet)
extern pthread_mutex_t lock;

// Need a mutex lock for printing because things are getting out of hand
pthread_mutex_t gagLock = PTHREAD_MUTEX_INITIALIZER;

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

// This function does not need mutex locks as it only modifies local variables
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

// This method will try to request resouces from the system
int request_Resources(int customer_Num, int request[]) {
	// Step 1: Check if Requesti ≤ Needi
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		if (request[i] > needRemaining[customer_Num][i]) {
			printf("Error: Customer %d has exceeded its maximum claim.\n", customer_Num+1);
			return -1; // Error: Process has exceeded its maximum claim
		}
	}

	// Step 1.a: Check if Requesti ≤ Availablei
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		if (request[i] > available[i]) {
			// Step 1.a.i: Process must wait, as resources are not available
			printf("Customer %d must wait, as resources are not available.\n", customer_Num+1);
			return -1; // Resources are not available
		}
	}

	// If the program reaches here, it means Requesti ≤ Needi and Requesti ≤ Availablei
	pthread_mutex_lock(&availableLock); // So for clarification, I locked each array independently to
	pthread_mutex_lock(&allocatedCurrentlyLock);// ensure that arrays arent being change it other methods
	pthread_mutex_lock(&needRemainingLock);
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] -= request[i];
		allocatedCurrently[customer_Num][i] += request[i];
		needRemaining[customer_Num][i] -= request[i];
	}
	int a = are_We_Safe();
	if(a==0){ // Undo the allocation because system is not safe
		for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
			available[i] += request[i];
			allocatedCurrently[customer_Num][i] -= request[i];
			needRemaining[customer_Num][i] += request[i];
		} 
	}


	pthread_mutex_unlock(&needRemainingLock); // Convention for releasing the locks: reverse order
	pthread_mutex_unlock(&allocatedCurrentlyLock);
	pthread_mutex_unlock(&availableLock);

	printf("Request for Customer %d has been granted.\n", customer_Num + 1);
	return 0; // Request has been granted
}


int release_Resources(int customer_Num, int release[]){
	pthread_mutex_unlock(&availableLock);
	pthread_mutex_unlock(&allocatedCurrentlyLock);
	pthread_mutex_unlock(&needRemainingLock);
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] += release[i];
		allocatedCurrently[customer_Num][i] -= release[i];
		needRemaining[customer_Num][i] += release[i];
	}
	pthread_mutex_unlock(&needRemainingLock);
	pthread_mutex_unlock(&allocatedCurrentlyLock);
	pthread_mutex_unlock(&availableLock);
	return 0; // -1 if cant do, why should this ever be -1?
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

// Init. the maximum need array
void initMaximumNeed(){		
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
			maximumNeed[i][j] = 0;
			// no need for muxtex locks as this function is called only once and before the threads are created
		}
	}
}
// Must add a check to see if max need is all 0's (FIX ME) --> The generated row  
// This function will fill the max need array for each person that will not exceed the total resources for the system
void fillMaximumNeed() {
	srand(time(NULL));
	initMaximumNeed();
	bool allZero;
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		allZero = true;
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
			// Generate a random number in the range [0, maxSystemResources[j]]
			maximumNeed[i][j] = rand() % ((maxSystemResources[j])/(NUMBER_OF_CUSTOMERS/2) + 1);
			if(maximumNeed[i][j] != 0){
				allZero = false;
			}
		}
		if(allZero){
			i--;
		}
	}
}

// This will calculate the need remaining array
void calculateNeedRemaining() {
	// Don't need a lock for maximumNeed because thread function can't modify it
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

	int stuck = 0; // Ensure stuck is initialized
	bool allZero = true; // This is to start the loop

	while (stuck < 10) { // Loop up to 10 times
		allZero = true; // Reset allZero to true at the start of each iteration
		for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
			// Assuming needRemaining[rowIndex][i] is properly defined and accessible
			tempRow[i] = rand() % (needRemaining[rowIndex][i] + 1);
			if (tempRow[i] != 0) {
				allZero = false;
				break; // Exit the for-loop early as we have a non-zero request
			}
		}

		if (!allZero) {
			// Successfully generated a non-zero request, no need to try again
			break; // Exit the while-loop
		} else {
			//printf("All requests are zero, please make another request.\n");
			stuck++;
		}
	}

	if (stuck == 10) {
		printf("Tried 10 times with all zero requests.\n");
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
	printf("These are the number of each resource requested: ");
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		printf("%d ", tempRow[i]);
	}
}

// Main loop for serving customers, used to simulate normal operating conditions
// Ensure that each thread has its own id in the customer list 
void serviceCustomers() {
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {

		// pthread_mutex_lock(&); no need for mutex locks becuase this function is not seen by the threads
		// pthread_mutex_lock(&);
		printf("\nServicing customer %d\n", i + 1); // +1 to start counting customers from 1
		generateSmallerRow(i);
		printTempRow();
		printf("\n");
		// This displays the current request for the customer
		request_Resources(i,tempRow);
		printf("\nThis is the current allocated array\n");
		print2DArray(allocatedCurrently);

		// pthread_mutex_lock(&);
		// pthread_mutex_lock(&);

		sleep(2);
	}
}

// This function is responsible for releasing a random number of resources for a customer 
int *random_release(int rowIndex){

	printf("Random Release Array: ");
	for (int i = 0; i < NUMBER_OF_RESOURCES ; i++) {
		resources_to_release[i] = 0;
	}
	for (int i = 0; i < NUMBER_OF_RESOURCES ; i++) {
		if(allocatedCurrently[rowIndex][i] != 0) {
			resources_to_release[i] = 1;
			// break; 
		}

		printf("%d", resources_to_release[i]);
	}
	printf("\n");
	return resources_to_release;
}	



// This is the thread function, this represents a customer
void *threadFunction(void *arg) {
	int *myData = (int *)arg; // Assignment     
	srand(time(NULL) ^ (intptr_t)arg);// Seed
	int times = rand() % 10 + 1; // Generate a random number from 1 to 10

	for(int i = 0; i < times; i++) {// This for loop decides what the customer is going to be doing
		if (i % 2 == 0) {
			pthread_mutex_lock(&gagLock);
			// i is even
			printf("Thread %d, iteration %d: (A) Requesting random resources.\n", *myData, i + 1);
			// Add the code to request resources
			printf("\nServicing customer %d\n", (*myData)); // +1 to start counting customers from 1
			generateSmallerRow(*myData-1); // fix me 
			printTempRow();
			printf("\n");
			// This displays the current request for the customer
			request_Resources((*myData-1),tempRow);
			printf("\nThis is the current allocated array\n");
			print2DArray(allocatedCurrently);
			printf("\n");
			pthread_mutex_unlock(&gagLock);
		} else {
			pthread_mutex_lock(&resourceReleaseLock);
			// i is odd
			printf("Thread %d, iteration %d: (B) Releasing random resources.\n", *myData, i + 1);
			// Add the code to release resources
			int *arr = random_release(*myData-1);
			release_Resources(*myData - 1, arr);
			printf("\nThis is the current allocated array\n");
			print2DArray(allocatedCurrently);
			pthread_mutex_unlock(&resourceReleaseLock);
		}
		sleep(1); // Sleep for demonstration purposes; may be removed or adjusted as needed
	}
	sleep(10);
	// Preparing to release all resources allocated to this customer
	int release[NUMBER_OF_RESOURCES];

	// Populate the release array with the resources currently allocated to the customer
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		release[i] = allocatedCurrently[(*myData) - 1][i]; // Assuming allocatedCurrently is accessible
	}

	// Now call release_Resources for the customer
	// Current problem with releasing resources and printing (FIX ME)
	pthread_mutex_lock(&gagLock);
	release_Resources(*myData - 1, release);
	printf("Customer %d is finished\n", *myData);
	print2DArray(allocatedCurrently);
	pthread_mutex_unlock(&gagLock);

	return NULL;
}


#endif // headerArray

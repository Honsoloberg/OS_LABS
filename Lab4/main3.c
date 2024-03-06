/*
Nathan Perez 100754066
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include "MEMmanage.h"

// Define constraints for resources
#define TOTAL_MEMORY 960 // in Mbytes
#define MEMORY_FOR_RT 64  // in Mbytes
#define TOTAL_PRINTERS 2    // Total available printers
#define TOTAL_SCANNERS 1    // Total available scanners
#define TOTAL_MODEMS 1      // Total available modems
#define TOTAL_CDS 2         // Total available CD drives
#define MAX_PROCESSES 100 // Constraint, as stated in lab manual, Changed to 100 for testing


// Define all structures
//Structure for processes in MEMmanage.h

// Creating nessesary structure for the linked list
typedef struct Node {
    Process* process;
    struct Node* next;
} Node;

// Creates structure for holding resource information
typedef struct {
    int usedMemory;    // Currently used memory in Mbytes
    int usedPrinters;  // Currently used printers
    int usedScanners;  // Currently used scanners
    int usedModems;    // Currently used modems
    int usedCDs;       // Currently used CDs
} ResourceState;

// void getFilenameFromUser(char* filename, int max_length);
// void readProcesses(const char* filename, Process* processes[], int *size);
// void insertInOrder(Process* array[], Process* newProcess, int *size);
// void printProcesses(const Process* processes[], int size);
// void simulateProcessArrival(Process* processes, int size, Node** RealTimeQueue, Node** UserJobQueue, Node** PrioOne, Node** PrioTwo, Node** PrioThree, Node* DispatchList);

// void freeList(Node** head);
// bool checkAndAllocateResources(Process* process);
// void createLinkedListFromArray(Node** head, Process* processes[], int size);
// void displayList(Node* DispatchList);


// Start of Linked list fucntions

Node* createNode(Process* process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

void insertAtBeginning(Node** head, Process* process) {
    Node* newNode = createNode(process);
    newNode->next = *head;
    *head = newNode;
}

void insertAtEnd(Node** head, Process* process) {
    Node* newNode = createNode(process);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void deleteNode(Node** head, Process* process) {
    Node* temp = *head, *prev = NULL;
    if (temp != NULL && temp->process->arrivalTime == process->arrivalTime) {
        *head = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL && temp->process->arrivalTime != process->arrivalTime) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) return; // Process not found in the list
    prev->next = temp->next;
    free(temp);
}


void displayList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        printf("Process at front: Arrival Time: %d, Priority: %d, Processor Time: %d, Memory Requirement: %d\n", 
               temp->process->arrivalTime, temp->process->priority, temp->process->processorTime, temp->process->memoryRequirement);
        temp = temp->next;
    }
    printf("NULL\n");
}

void freeList(Node** head) {
    Node* temp;
    while (*head != NULL) {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

// End of Linked list fucntions


// File interpretation functions

// Function to prompt the user for the filename and store it
void getFilenameFromUser(char* filename, int max_length) {
    printf("Please enter name of Dispatcher list: ");// Ask the user for the filename
    fgets(filename, max_length, stdin);
    size_t len = strlen(filename); 
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
}

// Function to read text  file
void readProcesses(const char* filename, Process processes[], int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Process tempProcess;
    *size = 0; // Start with an empty array
    while (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                  &tempProcess.arrivalTime,
                  &tempProcess.priority,
                  &tempProcess.processorTime,
                  &tempProcess.memoryRequirement,
                  &tempProcess.printers,
                  &tempProcess.scanners,
                  &tempProcess.modems,
                  &tempProcess.cds) == 8) {
        insertInOrder(processes, tempProcess, size);
    }

    fclose(file);
}

// Function to insert
void insertInOrder(Process array[], Process newProcess, int *size) {
    int i = *size - 1;
    // Find the correct position for the new process
    while (i >= 0 && array[i].arrivalTime > newProcess.arrivalTime) {
        array[i + 1] = array[i]; // Shift elements to the right
        i--;
    }
    array[i + 1] = newProcess; // Insert the new process
    (*size)++; // Increase the size of the array
}

// Function to print the contents of the processes array, this is a test function, not needed in end program
void printProcesses(const Process processes[], int size) {
    printf("Listing all processes:\n");
    for (int i = 0; i < size; ++i) {
        printf("Process %d:\n", i + 1);
        printf("\tArrival Time: %d\n", processes[i].arrivalTime);
        printf("\tPriority: %d\n", processes[i].priority);
        printf("\tProcessor Time: %d\n", processes[i].processorTime);
        printf("\tMemory Requirement: %d\n", processes[i].memoryRequirement);
        printf("\tPrinters: %d\n", processes[i].printers);
        printf("\tScanners: %d\n", processes[i].scanners);
        printf("\tModems: %d\n", processes[i].modems);
        printf("\tCDs: %d\n\n", processes[i].cds);
    }
}

void createLinkedListFromArray(Node* DispatchList, Process* processes[], int size) {
    Process* p = NULL;
    for (int i = 0; i < size; i++) {
        p = processes[i];
        insertAtEnd(&DispatchList, p);
    }
}

// End of File interpretation functions


// Start of handling scheduling
void handleProcess(Process* process, Node** RealTimeQueue, Node** UserJobQueue, Node** PrioOne, Node** PrioTwo, Node** PrioThree, Node* DispatchList) {
    printf("Handling process with arrival time %d\n", process->arrivalTime);
    
    // Check the priority level of this new process
    if (process->priority == 0) {
        // Add this new process to RealTimeQueue
        insertAtEnd(RealTimeQueue, process);
    } else if (process->priority >= 1 && process->priority <= 3) {
        // Add this process to UserJobQueue
        insertAtEnd(UserJobQueue, process);
        
        // Check if the process has enough available resources to be run
        // if (checkAndAllocateResources(process)) {
        //     // Resources are subtracted from the total resources in checkAndAllocateResources
        //     // Add the process to the appropriate priority queue
        //     switch (process.priority) {
        //         case 1:
        //             insertAtEnd(PrioOne, process);
        //             break;
        //         case 2:
        //             insertAtEnd(PrioTwo, process);
        //             break;
        //         case 3:
        //             insertAtEnd(PrioThree, process);
        //             break;
        //     }
        // }
    }
}

void simulateProcessArrival(Node* Processor, int size, Node** RealTimeQueue, Node** UserJobQueue, Node** PrioOne, Node** PrioTwo, Node** PrioThree, Node* DispatchList) {
    for (int currentTime = 0; currentTime < MAX_PROCESSES; currentTime++) {
        printf("New Tick %d\n",currentTime);
        for (int i = 0; i < size; i++) {

            // Decrement processorTime of current running process in Processor

            if(Processor!=NULL){
                Processor->process->processorTime -= 1;
            }

            // Check if the first node in the DispatchList can be queue
            while(checkResource(DispatchList->process))
            {
                handleProcess(Processor, RealTimeQueue, UserJobQueue, PrioOne, PrioTwo, PrioThree, DispatchList);
            }

            // Function call to see if processor is being used by the highest priority funciton possible & to find new function for processor
            moveprocess(Processor, RealTimeQueue, UserJobQueue, PrioOne, PrioTwo, PrioThree, DispatchList);


        }
    }

}

// End of handling scheduling

// Start of the main

int main() {
    //MemoryBlock* root = initializeMemory(TOTAL_MEMORY); // Initialize with 1KB for testing

    // Initialize all queues
    Node* RealTimeQueue = NULL;
    Node* UserJobQueue = NULL;
    Node* PrioOne = NULL;
    Node* PrioTwo = NULL;
    Node* PrioThree = NULL;
    Node* Processor = NULL;
    Node* DispatchList = NULL;
    
    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    Process processes[MAX_PROCESSES];
    int size = 0;
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, processes, &size);// Call the function to process the file
    // Simple test function call to check if program was able to pull values
    //printProcesses(processes, size); // comment this out before hand in
    // After this runs, there will be an array holding all of the process that were collected from the text file
    createLinkedListFromArray(DispatchList, processes, &size);
    displayList(DispatchList);




    //simulateProcessArrival(processes, size, &RealTimeQueue, &UserJobQueue, &PrioOne, &PrioTwo, &PrioThree); // This simulates the scheduler receiveing tasks at specific intervals of ticks

    // Free all queues
    freeList(&RealTimeQueue);
    freeList(&UserJobQueue);
    freeList(&PrioOne);
    freeList(&PrioTwo);
    freeList(&PrioThree);
    freeList(&Processor);
    freeList(&DispatchList);

    return 0;
}

// End of the main, it is small, but the entire scheduluer finishes its job before all queues are freed
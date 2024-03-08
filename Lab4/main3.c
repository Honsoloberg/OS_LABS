/*
Nathan Perez 100754066
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <windows.h> //Comment out if running on Linux
#include <string.h>
#include <stdbool.h>
#include "MEMmanage.h"

// Define constraints for resources
#define MAX_PROCESSES 100 // Constraint, as stated in lab manual, Changed to 100 for testing


// Define all structures
//Structure for processes in MEMmanage.h

// Creating nessesary structure for the linked list
typedef struct Node {
    Process* process;
    struct Node* next;
} Node;

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
   printf("insert me please\n");
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
    printf("insert me please 2\n");
}



void deleteNode(Node **head) {
    if(*head == NULL) {
        printf("List is already empty.\n");
    } else {
        Node *toDelete = *head;
        *head = (*head)->next; 
        destroyJob(toDelete->process); // needs to be passed a process (process is the data in each node)
        free(toDelete); 
        printf("SUCCESSFULLY DELETED FIRST NODE FROM LIST\n");
    }
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

// Function to insert
void insertInOrder(Process *array[], Process* newProcess, int *size) {
    int i = *size - 1;
    // Find the correct position for the new process
    while (i >= 0 && array[i]->arrivalTime > newProcess->arrivalTime) {
        array[i + 1] = array[i]; // Shift elements to the right
        i--;
    }
    array[i + 1] = newProcess; // Insert the new process
    (*size)++; // Increase the size of the array
}

// Function to read text file and store processes in an array
void readProcesses(const char* filename, Process processes[], int *size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *size = 0; // Start with an empty array
    while (*size < 1000) { // Ensure we don't overflow the array, MAX_SIZE is the maximum capacity of 'processes'
        if (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                   &processes[*size].arrivalTime,
                   &processes[*size].priority,
                   &processes[*size].processorTime,
                   &processes[*size].memoryRequirement,
                   &processes[*size].printers,
                   &processes[*size].scanners,
                   &processes[*size].modems,
                   &processes[*size].cds) == 8) {
            // Successfully read a process, increment size
            (*size)++;
        } else {
            // If fscanf fails to read 8 items, break out of the loop
            // Also check if we've reached the end of the file or encountered an error
            if (feof(file)) {
                // End of file reached successfully
                break;
            } else if (ferror(file)) {
                // File read error encountered
                perror("Error reading file");
                break;
            }
        }
    }

    fclose(file);
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

void createLinkedListFromArray(Node** DispatchList, Process processes[], int size) {
    for (int i = 0; i < size; i++) {
        insertAtEnd(DispatchList, &processes[i]);
    }
}


// End of File interpretation functions


// Start of handling scheduling
// void handleProcess(Node** RealTimeQueue, Node** UserJobQueue, Node* DispatchList) {
//     printf("Handling process with arrival time %d\n", DispatchList->process->arrivalTime);
//     if (DispatchList->process->priority == 0) {
//         printf("handle 1\n");
//         insertAtEnd(RealTimeQueue, DispatchList->process);
//         deleteNode(&DispatchList);
//     } else if (DispatchList->process->priority > 0 && DispatchList->process->priority <= 3) {
//         printf("handle 2\n");
//         insertAtEnd(UserJobQueue, DispatchList->process);
//         printf("handle 3.5\n");
//         deleteNode(&DispatchList);
//         printf("handle 3\n");
//     }
// }

void handleProcess(Node** Queue, Node* DispatchList) {
    printf("Handling process with arrival time %d\n", DispatchList->process->arrivalTime);
    insertAtEnd(Queue, DispatchList->process);
    deleteNode(&DispatchList);
    
}

void enqueueUserJob(Node **UserJobQueue, Node** PrioOne, Node** PrioTwo, Node** PrioThree){
    switch((*UserJobQueue)->process->priority){
        case 1:{
            insertAtEnd(PrioOne, (*UserJobQueue)->process);
            break;
        }
        case 2:{
            insertAtEnd(PrioTwo, (*UserJobQueue)->process);
            break;
        }
        case 3:{
            insertAtEnd(PrioThree, (*UserJobQueue)->process);
            break;
        }
    }
    deleteNode(UserJobQueue);
}


void moveprocess(Node *Processor, Node* RealTimeQueue, Node* PrioOne, Node* PrioTwo , Node* PrioThree){
    if (Processor == NULL || Processor->process == NULL) {
        if(RealTimeQueue->next != NULL){
            insertAtBeginning(&Processor, RealTimeQueue->process);
            deleteNode(&RealTimeQueue);
        }else if(PrioOne->next != NULL){
            insertAtBeginning(&Processor, PrioOne->process);
            deleteNode(&PrioOne);
        }else if(PrioTwo->next != NULL){
            insertAtBeginning(&Processor, PrioTwo->process);
            deleteNode(&PrioTwo);
        }else{
            insertAtBeginning(&Processor, PrioThree->process);
            deleteNode(&PrioThree);
        }
    }
    switch(Processor->process->priority){
        case 0:
            return;
        case 1:{
            if(RealTimeQueue->next != NULL){
                insertAtEnd(&PrioTwo, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, RealTimeQueue->process);
                deleteNode(&RealTimeQueue);
            }
            break;
        }
        case 2:{
            if(RealTimeQueue->next != NULL){
                insertAtEnd(&PrioThree, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, RealTimeQueue->process);
                deleteNode(&RealTimeQueue);
            }else if(PrioOne->next != NULL){
                insertAtEnd(&PrioThree, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, PrioOne->process);
                deleteNode(&PrioOne);
            }
            break;
        }
        case 3:{
            if(RealTimeQueue->next != NULL){
                insertAtEnd(&PrioThree, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, RealTimeQueue->process);
                deleteNode(&RealTimeQueue);
            }else if(PrioOne->next != NULL){
                insertAtEnd(&PrioThree, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, PrioOne->process);
                deleteNode(&PrioOne);
            }else if(PrioTwo->next != NULL){
                insertAtEnd(&PrioThree, Processor->process);
                deleteNode(&Processor);
                insertAtBeginning(&Processor, PrioTwo->process);
                deleteNode(&PrioTwo);
            }
            break;
        }
    }

}


void simulateProcessArrival(Node* Processor, int size, Node** RealTimeQueue, Node** UserJobQueue, Node** PrioOne, Node** PrioTwo, Node** PrioThree, Node* DispatchList) {
    for (int currentTime = 0; currentTime < MAX_PROCESSES; currentTime++) {
        printf("New Tick %d\n",currentTime);
        for (int i = 0; i < size; i++) {

            if(Processor->process->processorTime == 0){
                destroyJob(Processor->process);
                Processor = NULL;
            }

            // Decrement processorTime of current running process in Processor
            if(Processor!=NULL){
                printf("hello there\n");
                Processor->process->processorTime -= 1;
            }
                printf("hello there 2\n");
            // Check if the first node in the DispatchList can be queue
            while(DispatchList->process->arrivalTime <= currentTime){
                if(DispatchList->process->priority == 0){
                    if(checkResource(DispatchList->process)){
                        Allocate(DispatchList->process);
                        handleProcess(RealTimeQueue, DispatchList);
                    }
                }else{
                    handleProcess(UserJobQueue, DispatchList);
                    if(checkResource((*UserJobQueue)->process)){
                        Allocate((*UserJobQueue)->process);
                        enqueueUserJob(UserJobQueue, PrioOne, PrioTwo, PrioThree);
                    }
                }
            }
            printf("hello there 4\n");
            // Function call to see if processor is being used by the highest priority funciton possible & to find new function for processor
            moveprocess(Processor, *RealTimeQueue, *PrioOne, *PrioTwo, *PrioThree);


        }
    }

}

// End of handling scheduling

// Start of the main

int main() {
    //MemoryBlock* root = initializeMemory(TOTAL_MEMORY); // Initialize with 1KB for testing

    // Initialize all queues, these are the heads of the queue
    
    Node* RealTimeQueue = NULL;
    Node* UserJobQueue = NULL;
    Node* PrioOne = NULL;
    Node* PrioTwo = NULL;
    Node* PrioThree = NULL;
    Node* Processor = NULL;
    Node* DispatchList = NULL;
    
    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    Process processes[1000];
    int size = 0;
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, processes, &size);// Call the function to process the file
    // Simple test function call to check if program was able to pull values
    printProcesses(processes, size); // comment this out before hand in
    // After this runs, there will be an array holding all of the process that were collected from the text file
    createLinkedListFromArray(&DispatchList, processes, size);
    displayList(DispatchList);




    simulateProcessArrival(Processor, size, &RealTimeQueue, &UserJobQueue, &PrioOne, &PrioTwo, &PrioThree, DispatchList); // This simulates the scheduler receiveing tasks at specific intervals of ticks

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
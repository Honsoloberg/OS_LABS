/*
Authors: Nathan Perez (100754066), Logan Butler (100828103), Dmitri Rios Nadeau (100783206), Jordan Frost Hagedorn(100828122)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> //Comment out if running on Linux
#include <string.h>
#include <stdbool.h>
#include "JobDescription.h"
#include "MEMmanage.h"

// This will fill in the information corresponding to the arrival time, priority, and the process time
void fillProcessInfo(Process* process, int arrivalTime, int priority, int processorTime){
process->arrivalTime=arrivalTime;
process->priority=priority;
process->processorTime=processorTime;
}

// This will fill in the remaining information for the process regarding the resources
void fillResourceInfo(Resources* resources, int memoryRequirement, int printers, int scanners, int modems, int cds){
resources->memoryRequirement=memoryRequirement;
resources->printers=printers;
resources->scanners=scanners;
resources->modems=modems;
resources->cds=cds;
}

// Create Job method will create a new job, Will return the job
Job* createJob(int arrivalTime, int priority, int processorTime, int memoryRequirement, int printers, int scanners, int modems, int cds) {
    Job* newJob = (Job*)malloc(sizeof(Job));
    if (newJob == NULL) {
        printf("Error creating a new job.\n"); 
        exit(0);
    }
newJob->next = NULL;
fillProcessInfo(&newJob->processInfo, arrivalTime, priority, processorTime);
fillResourceInfo(&newJob->processResources, memoryRequirement, printers, scanners, modems, cds);
newJob->currentRunningTimeOnProcessor = 0;
return newJob;
}

// Function to prompt the user for the filename and store it
void getFilenameFromUser(char* filename, int max_length) {
    printf("Please enter name of Dispatcher list: ");// Ask the user for the filename
    fgets(filename, max_length, stdin);
    size_t len = strlen(filename); 
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
}

// This function will insert jobs into the dispatch list in order of arrival time
void insertSortedByArrivalTime(Job** headRef, Job* newJob) {
    Job* current;
    // Special case for the head end
    if (*headRef == NULL || (*headRef)->processInfo.arrivalTime >= newJob->processInfo.arrivalTime) {
        newJob->next = *headRef;
        *headRef = newJob;
    } else {
        // Locate the node before the point of insertion
        current = *headRef;
        while (current->next != NULL && current->next->processInfo.arrivalTime < newJob->processInfo.arrivalTime) {
            current = current->next;
        }
        newJob->next = current->next;
        current->next = newJob;
    }
}

// Will read in processes from the text file, then call a method to insert it into the dispatch list
void readProcesses(const char* filename, Job** dispatchList) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int arrive, priority, dur, MEM, print, scan, mod, cds;
    while (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n", &arrive, &priority, &dur, &MEM, &print, &scan, &mod, &cds) == 8) {
        Job* newJob = createJob(arrive, priority, dur, MEM, print, scan, mod, cds);
        insertSortedByArrivalTime(dispatchList, newJob);
    }

    fclose(file);
}

// This method will display all of the contents of the linked list
void displayList(Job* head) {
    int count = 1;
    Job* temp = head;
    while (temp != NULL) {
        printf("Process at front %d: Arrival Time: %d, Priority: %d, Processor Time: %d, Memory Requirement: %d\n", 
               count, temp->processInfo.arrivalTime, temp->processInfo.priority, temp->processInfo.processorTime, temp->processResources.memoryRequirement);
        temp = temp->next;
        count++;
    }
    printf("\n");
}

// This function will return a true or false value that will indicate wether or not there is enough resources on the system
// For the process
int checkResource(Job* job, SystemResourcesUsed* TotalUsedResources){//Checks if sufficient resources are available
    // This is specific for priority 0 jobs, since they have their own dedicated memory 
    if(job->processInfo.priority == 0){
        if((job->processResources.memoryRequirement + TotalUsedResources->rtMEM) <= RT_MEM){
            return 1;
        }else{
            return 0;
        }
    }else{
        if(((job->processResources.memoryRequirement + TotalUsedResources->UserMEM) <= MAX_USER_MEM)
        && (job->processResources.printers + TotalUsedResources->printers) <= MAX_PRINT 
        && (job->processResources.scanners + TotalUsedResources->scanners) <= MAX_SCAN 
        && (job->processResources.modems + TotalUsedResources->modems) <= MAX_MODEM 
        && (job->processResources.cds + TotalUsedResources->cds) <= MAX_CD){
            return 1;
        }else{
            return 0;
        }
    }
}

// This function uses the system variables initialized to 0, and adds to them to indicate that the resources are being used 
void Allocate(Job* job, SystemResourcesUsed* TotalUsedResources){
    if(job->processInfo.priority == 0){
        TotalUsedResources->rtMEM += job->processResources.memoryRequirement;
        return;
    }
    TotalUsedResources->UserMEM+= job->processResources.memoryRequirement;
    TotalUsedResources->printers+= job->processResources.printers;
    TotalUsedResources->scanners+= job->processResources.scanners;
    TotalUsedResources->modems+= job->processResources.modems;
    TotalUsedResources->cds+= job->processResources.cds;
}

// Will return a pointer to the new head
Job* JobRemoveHead(Job** head){
if(*head==NULL){
    // List is empty
    return NULL;
}
Job* temp;
temp = (*head);
(*head) = temp->next;
temp->next = NULL; // Very important, without this the entire list will be yoinked with it
return temp;
}

// This function adds the job to the end of the linked list 
void JobInsertAtEnd(Job** head, Job* newJob) {
    if (*head == NULL) {
        *head = newJob;
        return;
    }
    Job* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newJob;
}

// This function moves a job from the front of one linked list to the end of another linked list
void MoveJob(Job** sourceList, Job** destinationList) {
    Job* JobPointer = JobRemoveHead(sourceList);
    JobInsertAtEnd(destinationList, JobPointer);
}

// This function pulls from the User Job queue and puts them onto the appropirate priority queue 
void enqueueUserJob(Job** UserJobQueue, Job** PrioOne, Job** PrioTwo, Job** PrioThree){
    switch((*UserJobQueue)->processInfo.priority){
        case 1:{
            MoveJob(UserJobQueue, PrioOne);
            break;
        }
        case 2:{
            MoveJob(UserJobQueue, PrioTwo);
            break;
        }
        case 3:{
            MoveJob(UserJobQueue, PrioThree);
            break;
        }
    }
}

// This function will decide wether or not to pull tasks from the processor and which task to add to it
void moveprocess(Job** Processor, Job** RealTimeQueue, Job**  PrioOne, Job** PrioTwo , Job** PrioThree, QuantumTime* QTime){
    // If there is nothing in the processor, pull from one of the queues
    if (*Processor == NULL) {
        if(*RealTimeQueue != NULL){
            MoveJob(RealTimeQueue, Processor); 
        }else if(*PrioOne != NULL){
            MoveJob(PrioOne, Processor);
        }else if(*PrioTwo!= NULL){
            MoveJob(PrioTwo, Processor);
        }else if(*PrioThree!= NULL){
             MoveJob(PrioThree, Processor);
        } 
        else{
            // This will never be printed
            printf("ERROR, Very bad if it gets here"); 
            return;
        }
    }else {
        // Checks if current running process is not priority 0, because priority 0 jobs run till conmpletion 
            if(((*Processor)->processInfo.priority != 0)
            && ((*Processor)->processInfo.priority <= 3))
            {
                // This checks if the current running process has used up all its quantum time
                switch((*Processor)->processInfo.priority){
                case 1: {
                        if((*Processor)->currentRunningTimeOnProcessor<QTime->prioOne){
                        break;
                        }
                        (*Processor)->processInfo.priority += 1;
                        (*Processor)->currentRunningTimeOnProcessor = 0;
                        break;
                        }
                case 2: {
                        if((*Processor)->currentRunningTimeOnProcessor<QTime->prioTwo){
                        break;
                        }
                        (*Processor)->processInfo.priority += 1;
                        (*Processor)->currentRunningTimeOnProcessor = 0;
                        break;
                        }
                case 3: {
                        if((*Processor)->currentRunningTimeOnProcessor<QTime->prioThree){
                        break; 
                        }
                        (*Processor)->currentRunningTimeOnProcessor = 0;
                        break;
                        }                        
                }
            }
        // This switch will chose the correct process to take over the processor and reset the quantum time for that process
        switch((*Processor)->processInfo.priority){
            case 0:
                break;
            case 1:
            if(*RealTimeQueue != NULL){
                   (*Processor)->currentRunningTimeOnProcessor = 0;
                    MoveJob(Processor, PrioOne);
                    MoveJob(RealTimeQueue, Processor);
                break;
            case 2:{
                if(*RealTimeQueue != NULL){
                    (*Processor)->currentRunningTimeOnProcessor = 0;
                    MoveJob(Processor, PrioTwo);
                    MoveJob(RealTimeQueue, Processor);
                }else if(*PrioOne != NULL){
                    (*Processor)->currentRunningTimeOnProcessor = 0;
                    MoveJob(Processor, PrioTwo);
                    MoveJob(PrioOne, Processor);
                }
                break;
            }
            case 3:{
                if(*RealTimeQueue != NULL){
                    (*Processor)->currentRunningTimeOnProcessor = 0;
                    MoveJob(Processor, PrioThree);
                    MoveJob(RealTimeQueue, Processor);
                }else if(*PrioOne != NULL){
                    (*Processor)->currentRunningTimeOnProcessor = 0;
                MoveJob(Processor, PrioThree);
                    MoveJob(PrioOne, Processor);
                }else if(*PrioTwo != NULL){
                    (*Processor)->currentRunningTimeOnProcessor = 0;
                    MoveJob(Processor, PrioThree);
                    MoveJob(PrioTwo, Processor);
                }
                break;
                
            }
        }
    }
}
}

// Will deallocate the resources, for our system, that means subtracting the resources from the total used resources
void DeAllocate(Job* job, SystemResourcesUsed* TotalUsedResources){
    if(job->processInfo.priority == 0){
        TotalUsedResources->rtMEM -= job->processResources.memoryRequirement;
        return;
    }
    TotalUsedResources->UserMEM-= job->processResources.memoryRequirement;
    TotalUsedResources->printers-= job->processResources.printers;
    TotalUsedResources->scanners-= job->processResources.scanners;
    TotalUsedResources->modems-= job->processResources.modems;
    TotalUsedResources->cds-= job->processResources.cds;
}

// This function deletes a job, and frees its resources
void deleteJob(Job** head,  SystemResourcesUsed* TotalUsedResources) {
    if(*head == NULL) {
        printf("List is already empty. This is very bad.\n");
    } else {

    Job* JobPointer = JobRemoveHead(head);

        DeAllocate(JobPointer,TotalUsedResources); 
        free(JobPointer); 
        printf("SUCCESSFULLY DELETED FIRST NODE FROM LIST\n\n");
    }
}

// This is the main function the simulates the passing of time, every "tick", new processes are read from the dispatch list and added to 
// The appropriate queues
void simulateProcessArrival(Job** Processor, Job** RealTimeQueue, Job** UserJobQueue, Job** PrioOne, Job** PrioTwo, Job** PrioThree, Job** DispatchList, SystemResourcesUsed* TotalUsedResources, QuantumTime* QuantumTime) {
    for (int currentTime = 0; currentTime < MAX_PROCESSES_TIME; currentTime++) {
        printf("\n\n\n\nNew Tick %d\n",currentTime);

        // Processor does work, and deletes it if it finishes
            if((*Processor) != NULL){
                (*Processor)->processInfo.processorTime -= 1;
                (*Processor)-> currentRunningTimeOnProcessor += 1;
                if((*Processor)->processInfo.processorTime == 0){
                    printf("We did it, task completed\n");
                    displayList(*Processor);
                    deleteJob(Processor,TotalUsedResources);
                }
            }

        Job* tempJob = *DispatchList;
            // While there is still jobs in the dispatch list, this will continue to run every tick
            while(tempJob != NULL && tempJob->processInfo.arrivalTime <= currentTime){
                if(tempJob->processInfo.priority == 0){
                    // Re-name allocate to can process
                        if(checkResource(tempJob,TotalUsedResources)){
                            Allocate(tempJob,TotalUsedResources);
                            MoveJob(DispatchList, RealTimeQueue);
                        }
                }else{
                    MoveJob(DispatchList, UserJobQueue);
                    if(checkResource(tempJob, TotalUsedResources)){
                        Allocate(tempJob,TotalUsedResources);
                        enqueueUserJob(UserJobQueue, PrioOne, PrioTwo, PrioThree);
                    }
                }
                tempJob = *DispatchList;
            }
            
            // Function call to see if processor is being used by the highest priority funciton possible & to find new function for processor
            if(*RealTimeQueue != NULL || *PrioOne != NULL || *PrioTwo != NULL || *PrioThree != NULL){
                moveprocess(Processor, RealTimeQueue, PrioOne, PrioTwo, PrioThree, QuantumTime);
            }else if(*Processor == NULL){
               printf("Processor waits\n");
            }
           // This will print out all of the queues in the system
            printf("End of Tick Lists\n");
            printf("DispatchList\n");
            displayList(*DispatchList);
            printf("Processor\n");
            displayList(*Processor);
            printf("RealTimeQueue\n");
            displayList(*RealTimeQueue);
            printf("UserJobQueue\n");
            displayList(*UserJobQueue);
            printf("PrioOne\n");
            displayList(*PrioOne);
            printf("PrioTwo\n");
            displayList(*PrioTwo);
            printf("PrioThree\n");
            displayList(*PrioThree);  
    }
}

// This is the main
int main(){

    // Initialize the queues 
    Job* RealTimeQueue = NULL;
    Job* UserJobQueue = NULL;
    Job* PrioOne = NULL;
    Job* PrioTwo = NULL;
    Job* PrioThree = NULL;
    Job* Processor = NULL;
    Job* DispatchList = NULL;

// Initializes the used resources to zero
    SystemResourcesUsed TotalUsedResources; 
    TotalUsedResources.rtMEM = 0;
    TotalUsedResources.UserMEM = 0;
    TotalUsedResources.printers = 0;
    TotalUsedResources.scanners = 0;
    TotalUsedResources.modems = 0;
    TotalUsedResources.cds = 0;

// This is what dictates how long a process will run at each priority level
    QuantumTime QTime; 
    QTime.prioOne = 4;
    QTime.prioTwo = 6;
    QTime.prioThree = 8;

    // All the File stuff
    char filename[256]; // Buffer to hold the filename
    int size;
    
    getFilenameFromUser(filename, sizeof(filename)); // Call the function to get the filename from the user
    readProcesses(filename, &DispatchList);// Call the function to process the file
    simulateProcessArrival(&Processor, &RealTimeQueue, &UserJobQueue, &PrioOne, &PrioTwo, &PrioThree, &DispatchList, &TotalUsedResources, &QTime);

}
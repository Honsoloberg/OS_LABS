/*
Authors: Nathan Perez (100754066), Logan Butler (100828103), Dmitri Rios Nadeau (100783206), Jordan Frost Hagedorn(100828122)
*/

typedef struct QuantumTime{
    int prioOne; // Smallest
    int prioTwo; // Just the right size
    int prioThree; // Largest
}QuantumTime;

// Resources for the job
typedef struct Resources{
    int memoryRequirement;
    int printers;
    int scanners;
    int modems;
    int cds;
}Resources;

// Resources for the system that are being used
typedef struct SystemResourcesUsed{
    int rtMEM;
    int UserMEM;
    int printers;
    int scanners;
    int modems;
    int cds;
}SystemResourcesUsed;

// Arrival time, priority, and processor time for the job
typedef struct Process{
    int arrivalTime;
    int priority;
    int processorTime;
}Process;

// Job structure 
typedef struct Job {
     struct Job* next;
     Process processInfo;
     Resources processResources;
     int currentRunningTimeOnProcessor;
} Job;


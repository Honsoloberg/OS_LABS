/*
Author: Logan Butler
Student Number: 100828103
*/

#include <stdlib.h>
#include <stdio.h>

//This value is the maxium memory of jobs in MBytes
#define MAX_MEM 960
#define RT_MEM 64
#define MAX_SCAN 1
#define MAX_PRINT 2
#define MAX_MODEM 1
#define MAX_CD 2

//Resource Variables
unsigned int MEM = 0; //User Process Memory
unsigned int MEM_RT = 0; //Real-time Process Memory
unsigned int T_scanners = 0;
unsigned int T_modems = 0;
unsigned int T_cds = 0;
unsigned int T_printers = 0;

//Process Structure
typedef struct Process{
    int arrivalTime;
    int priority;
    int processorTime;
    int memoryRequirement;
    int printers;
    int scanners;
    int modems;
    int cds;
}Process;

//Create Job Variables
Process *makeJob(int time, int p, int dur, int size, int printers, int scanners, int modems, int cds){

    Process *job = (Process*) malloc(sizeof(Process));

    if(p == 0){//Setup for a Real-Time Job
        if((MEM_RT + size) <= RT_MEM){

            if(time >= 0){
                job->arrivalTime = time;
            }
            if(p >= 0){
                job->priority = p;
            }
            if(dur > 0){
                job->processorTime = dur;
            }
            if(size > 0){
                job->memoryRequirement = size;
            }

            return job;
        }
    }

    //Setup for a User Job

    if(time >= 0){
        job->arrivalTime = time;
    }
    if(p >= 0){
        job->priority = p;
    }
    if(dur > 0){
        job->processorTime = dur;
    }
    if(size > 0){
        job->memoryRequirement = size;
    }
    if(printers >= 0){
        job->printers = printers;
    }
    if(scanners >= 0){
        job->scanners = scanners;
    }
    if(modems >= 0){
        job->modems = modems;
    }
    if(cds >= 0){
        job->cds = cds;
    }
        
    return job;


}

int checkResource(Process *job){//Checks if sufficient resources are available
    if(job->priority == 0){
        if((job->memoryRequirement + MEM_RT) <= RT_MEM){
            return 1;
        }else{
            return 0;
        }
    }else{
        if((job->memoryRequirement + MEM) <= MAX_MEM && (job->printers+T_printers) <= MAX_PRINT && (job->scanners + T_scanners) <= MAX_SCAN && (job->modems + T_modems) <= MAX_MODEM && (job->cds + T_cds) <= MAX_CD){
            return 1;
        }else{
            return 0;
        }
    }
}

void Allocate(Process *job){
    if(job->priority == 0){
        MEM_RT+= job->memoryRequirement;
        return;
    }

    MEM+= job->memoryRequirement;
    T_printers+= job->printers;
    T_scanners+= job->scanners;
    T_modems+= job->modems;
    T_cds+= job->cds;

}

//Destroy Job Variables
void destroyJob(Process *job){
    if(job->priority == 0){
        MEM_RT-=job->memoryRequirement;
        free(job);
        return;
    }

    MEM -= job->memoryRequirement;
    T_printers-=job->printers;
    T_scanners-=job->scanners;
    T_modems-=job->modems;
    T_cds-=job->cds;

    free(job);
    return;
}
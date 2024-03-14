/*
Authors: Nathan Perez (100754066), Logan Butler (100828103), Dmitri Rios Nadeau (100783206), Jordan Frost Hagedorn(100828122)
*/

#include <stdlib.h>
#include <stdio.h>


//This value is the maxium memory of jobs in MBytes
#define TOTAL_MEM 1024
#define RT_MEM 64
#define MAX_USER_MEM (TOTAL_MEM-RT_MEM)

#define MAX_SCAN 1
#define MAX_PRINT 2
#define MAX_MODEM 1
#define MAX_CD 2

// Define constraints for resources
#define MAX_PROCESSES_TIME 20 // Constraint, Changed to 20 for testing. This number dictates how many ticks the program will run for


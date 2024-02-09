#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_THREADS 11
#define GRID_SIZE 9

int sudoku[9][9] = { 
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
};

typedef struct {
    int row;
    int col;
    int value;
} parameters;

bool check_numbers(int checker[9]) {
    for (int i = 0; i < 9; i++) {
        if (checker[i] != 1) return false;
    }
    return true;
}

// Thread function to check all rows
void* checkRow(void* param) {
    int (*sudoku)[9] = param;
    bool* result = malloc(sizeof(bool));
    *result = true;

    for (int i = 0; i < 9; i++) {
        int checker[9] = {0};
        for (int j = 0; j < 9; j++) {
            if (sudoku[i][j] < 1 || sudoku[i][j] > 9 || checker[sudoku[i][j] - 1] == 1) {
                *result = false;
                return result;
            }
            checker[sudoku[i][j] - 1] = 1;
        }
    }
    return result;
}

// Thread function to check all columns
void* checkColumn(void* param) {
    int (*sudoku)[9] = param;
    bool* result = malloc(sizeof(bool));
    *result = true;

    for (int i = 0; i < 9; i++) {
        int checker[9] = {0};
        for (int j = 0; j < 9; j++) {
            if (sudoku[j][i] < 1 || sudoku[j][i] > 9 || checker[sudoku[j][i] - 1] == 1) {
                *result = false;
                return result;
            }
            checker[sudoku[j][i] - 1] = 1;
        }
    }
    return result;
}

// Thread function to check all 3x3 subgrids
void* checkSubGrid(void* param) {
    parameters* p = (parameters*)param;
    bool* result = malloc(sizeof(bool));
    *result = true;

    int startRow = p->row;
    int startCol = p->col;
    int checker[9] = {0};

    for (int i = startRow; i < startRow + 3; i++) {
        for (int j = startCol; j < startCol + 3; j++) {
            if (sudoku[i][j] < 1 || sudoku[i][j] > 9 || checker[sudoku[i][j] - 1] == 1) {
                *result = false;
                return result;
            }
            checker[sudoku[i][j] - 1] = 1;
        }
    }
    return result;
}


int main() {
    pthread_t threads[NUM_THREADS];
    int threadIndex = 0;
    bool valid = true;

    // Row & column checks
    pthread_create(&threads[threadIndex++], NULL, checkRow, (void*)sudoku);
    pthread_create(&threads[threadIndex++], NULL, checkColumn, (void*)sudoku);

    // Creating nine threads for each 3x3 subgrid validation
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            parameters *data = (parameters *)malloc(sizeof(parameters));
            data->row = i * 3;
            data->col = j * 3;
            pthread_create(&threads[threadIndex++], NULL, checkSubGrid, (void*)data);
        }
    }

    // Waiting for all threads to complete
    for(int i = 0; i < NUM_THREADS; i++) {
        void* thread_result;
        pthread_join(threads[i], &thread_result);
        valid &= *(bool*)thread_result;
        free(thread_result);
    }

    // Check if Sudoku is valid
    if (valid) {
        printf("Sudoku is valid.\n");
    } else {
        printf("Sudoku is invalid.\n");
    }
    
    return 0;
}

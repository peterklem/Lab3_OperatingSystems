/*
Peter Klembczyk
Lab 3
Operating Systems

This code takes a 9x9 solved sudoku board and checks to see if it is a valid board.
This is accomplished by using three threads to check different paramenters. One thread
checks each row for repeated numbers, one checks columns, and one checks 3x3 boxes.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 9
// Struct definintion
typedef struct {
    int row, col;
}coordinates;

// Function prototypes
void* searchRow(void* coords);
void* searchCol(void* coords);
void* searchBox(void* coords);

// Global variables
int sudokuGrid[GRID_SIZE][GRID_SIZE] = { {5,3,4,6,7,8,9,1,2}, {6,7,2,1,9,5,3,4,8}, {1,9,8,3,4,2,5,6,7}, {8,5,9,7,6,1,4,2,3}, {4,2,6,8,5,3,7,9,1}, {7,1,3,9,2,4,8,5,6}, {9,6,1,5,3,7,2,8,4}, {2,8,7,4,1,9,6,3,5}, {3,4,5,2,8,6,1,7,9}   };
int resultArray[3] = {0,0,0}; // 1 if pass, -1 if fail, 0 if incomplete
int main(int argc, char* argv[])
{
    pthread_t threads[3]; 
    coordinates* coordArray[3]; //[0] is row checker, [1] is col checker, [2] is box checker
    int flag = 0; // True if an error is detected in resultArray
    
    // Check that all numbers are between 1 and 9
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            if (sudokuGrid[i][j] < 1 || sudokuGrid[i][j] > 9)
            {
                printf("Invalid number entered. Row: %d, Col: %d, value: %d\n",
                            i, j, sudokuGrid[i][j]);
                return -1;
            }
            
        }
    }
    // Print sudoku board

    printf("Sudoku board: \n");
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            printf("%d ", sudokuGrid[i][j]);
        }
        printf("\n");
    }
    // Allocate memory and initialize coordinates
    for (int i = 0; i < 3; i++)
    {
        coordArray[i] = malloc(sizeof(coordinates));
        coordArray[i]->row = 0; 
        coordArray[i]->col = 0;
        // Initialize threads
        
    }
    if(pthread_create(&threads[0], NULL, searchRow, coordArray[0])) // init row checker
    {
        printf("Row checker thread did not initialize properly, exiting.\n");
        return -1;
    }
    else if (pthread_create(&threads[1], NULL, searchCol, coordArray[1]))
    {
        printf("Col checker thread did not initialize properly, exiting.\n");
        return -1;
    }
    else if (pthread_create(&threads[2], NULL, searchBox, coordArray[2]))
    {
        printf("Box checker thread did not initialize properly, exiting.\n");
        return -1;
    }

    // Join threads
    for(int i = 0; i < 3; i++)
    {
        if(pthread_join(threads[i], NULL))
        {
            printf("Thread %d failed to join, exiting.\n", i);
            return -1;
        }
    }
    
    // Check result array
    for(int i = 0; i < 3; i++)
    {
        if (resultArray[i] == 1) // No errors
        {
            printf("Thread %d found no conflicts.\n", i);
        }
        else if(resultArray[i] < 0) // Errors found
        {
            printf("Thread %d found a conflict.\n", i);
            flag = 1;
        }
        else // Value not modified
        { 
            printf("Unexpected error with thread %d.\n", i);
            flag = 1;
        }
    }

    // Notify user of pass/fail
    if(flag)
        printf("Failure! This is an invalid Sudoku board.\n");
    else
        printf("Success! This is a valid Sudoku board.\n");
    return 0;
}

void* searchRow(void* coords)
{ // Searches each row left to right for repeating numbers
    int numsUsed[GRID_SIZE] = {0,0,0,0,0,0,0,0,0}; // flip to 1 once the corresponding number is used
    coordinates* c = coords;
    for(c->row = 0; c->row < GRID_SIZE; c->row++) // Check down each row
    {
        for(c->col = 0; c->col < GRID_SIZE; c->col++) // Check each col
        {
            if (numsUsed[sudokuGrid[c->row][c->col] - 1] == 0) // Number has not been used on row yet
            {
                numsUsed[sudokuGrid[c->row][c->col] - 1] = 1; 
            }
            else // Number is duplicate, invalid puzzle
            {
                resultArray[0] = -1;
                printf("Row conflict found. Row: %d, Col: %d, number: %d\n",
                                c->row+1, c->col+1, sudokuGrid[c->row][c->col]);
                return NULL;
            }
        }
        // Reset numsUsed for next row
        for(int i = 0; i < GRID_SIZE; i++) numsUsed[i] = 0;
    }
    // All rows have been verified
    resultArray[0] = 1;
    pthread_exit(NULL);

}

void* searchCol(void* coords)
{ // Searches each column top to bottom for repeating numbers
    int numsUsed[GRID_SIZE] = {0,0,0,0,0,0,0,0,0}; // flip to 1 once the corresponding number is used
    coordinates* c = coords;
    for(c->col = 0; c->col < GRID_SIZE; c->col++) // Check down each column
    {
        for(c->row = 0; c->row < GRID_SIZE; c->row++) // Check each col
        {   
            if (numsUsed[sudokuGrid[c->row][c->col] - 1] == 0) // Number has not been used on col yet, add 1 to adjust for 0-index
            {
                numsUsed[sudokuGrid[c->row][c->col] - 1] = 1; 
            }
            else // Number is duplicate, invalid puzzle
            {
                resultArray[1] = -1;
                printf("Column conflict found. Row: %d, Col: %d, number: %d\n",
                                c->row+1, c->col+1, sudokuGrid[c->row][c->col]);
                return NULL;
            }
        }
        // Reset numsUsed for next row
        for(int i = 0; i < GRID_SIZE; i++) numsUsed[i] = 0;
    } 
    // All columns have been verified
    resultArray[1] = 1; 
    pthread_exit(NULL);  
}

void* searchBox(void* coords)
{ // Searches each 3x3 box on the Sudoku grid for repeating numbers
    int numsUsed[GRID_SIZE] = {0,0,0,0,0,0,0,0,0}; // flip to 1 once the corresponding number is used
    coordinates* c = coords;

    for(int i = 0; i < 3; i++) // 3 gridboxes tall
    {
        for(int j = 0; j < 3; j++) // 3 gridboxes wide
        {
            for (int k = c->row; k < c->row + 3; k++)
            {
                for (int m = c->col; m < c->col + 3; m++)
                {
                    if (numsUsed[sudokuGrid[k][m] - 1] == 0) // Number has not been used on col yet, add 1 to adjust for 0-index
                    {
                        numsUsed[sudokuGrid[k][m] - 1] = 1; 
                    }
                    else // Number is duplicate, invalid puzzle
                    {
                        resultArray[2] = -1;
                        printf("Box conflict found. Row: %d, Col: %d, number: %d\n",
                                k+1, m+1, sudokuGrid[k][m]);
                        return NULL;
                    }
                }
            }
            // Reset numsUsed for next row
            for(int i = 0; i < GRID_SIZE; i++) numsUsed[i] = 0;
            c->col += 3; // Move to the box to the right
        }
        // Move down 3 rows, and all the way to the first column
        c->col = 0;
        c->row += 3;
    }
    // All columns have been verified
    resultArray[2] = 1;
    pthread_exit(NULL); 
}
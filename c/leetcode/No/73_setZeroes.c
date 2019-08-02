#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void setZeroes(int** matrix, int matrixRowSize, int matrixColSize) {
    int *row = calloc(matrixRowSize, sizeof(int));
    int *col = calloc(matrixColSize, sizeof(int));

    int i,j;
    for(i = 0; i < matrixRowSize; i++) {
        for(j = 0; j < matrixColSize; j++) {
            if(matrix[i][j] == 0) {
                row[i] |= 1;
                col[j] |= 1;
            }
        }
    }
    for(i = 0; i < matrixRowSize; i++) {
        if(row[i])
            memset(matrix[i], 0, matrixColSize * sizeof(int));
    }
    for(j = 0; j < matrixColSize; j++) {
        if(col[j]) {
            for(i = 0; i < matrixRowSize; i++) {
                matrix[i][j] = 0;
            }
        }
    }
}

int main(int argc, char *argv[])
{

}
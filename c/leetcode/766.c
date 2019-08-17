#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * ! https://leetcode-cn.com/problems/toeplitz-matrix/
 */

static void _find_the_init_point(int base, int *i, int *j) 
{
    if(base >= 0) {
        *j = 0;
        *i = base;
    } else {
        for(*i = base, *j = 0; base < 0; ++base, ++*i, ++*j);
    }
}

bool isToeplitzMatrix(int** matrix, int matrixSize, int* matrixColSize)
{
    // printf("%d %d\n", matrixSize, *matrixColSize);
    
    if(matrixSize == 1 || *matrixColSize == 1) {
        return true;
    }
    int i , j, k;

    #if 0
    for(i = 0; i < matrixSize; i++) {
        for(j = 0; j < *matrixColSize; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
    #endif
    
    int size = matrixSize + *matrixColSize - 3;
    for(i = 0; i < size; i++) {
        _find_the_init_point(matrixSize - 2 - i, &j, &k); 
        int tmp = matrix[j++][k++];
        // printf("tmp %d\t", tmp);
        for(; j < matrixSize && k < *matrixColSize; j++, k++) {
            // printf("%d\t", matrix[j][k]);
            if(tmp != matrix[j][k]) return false;
        }
        // printf("\n");
    }
    return true;
}


int main(int argc, char *argv[])
{
    int i, j, k;
    for(i = 3; i > -3; --i) {
        _find_the_init_point(i, &j, &k);
        printf("%d -> (%d, %d)\n", i, j, k);
    }
    return 0;
}
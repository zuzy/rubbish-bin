#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* findDiagonalOrder(int** matrix, int matrixRowSize, int matrixColSize, int* returnSize) {
    *returnSize = matrixRowSize * matrixColSize;
    int *ret = (int *)malloc(*returnSize * sizeof(int));
    int *p = ret;
    int i, j, n;
    i = j = n = 0;
    while(i < matrixColSize && j < matrixRowSize) {
        *p++ = matrix[j][i];
        n = i + j;
 
        if(n % 2) {
            --i;
            ++j;
            if(j == matrixRowSize) {
                --j;
                i += 2;
            }
            if(i < 0) {
                ++i;
            }
        } else {
            ++i;
            --j;
            if(i == matrixColSize) {
                --i;
                j += 2;
            }
            if(j < 0) {
                ++j;
            }
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int **s = (int **)calloc(3, sizeof(int *));
    int i = 0;
    int j = 0;
    for(i = 0; i < 3; i++) {
        s[i] = calloc(3, sizeof(int));
        for(j = 0; j < 3; j++) {
            s[i][j] = i*3+j;
            printf(" i + j = %d\n", i*3 + j);
        }
    }
    
    printf("sdf\n");
    int len;
    int *ret = findDiagonalOrder(s, 3,3,&len);
    for(i = 0; i < len; ++i) {
        printf("%d --> %d\n", i, ret[i]);
    }
    return 0;
}
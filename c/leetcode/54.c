#include <stdio.h>
#include <stdlib.h>


/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

int *build(int **matrix, int matrixSize, int matrixColSize, int **ret, int x, int y)
{
    int *p = *ret;
    int i = 0;
    for(; i < matrixColSize; i++) {
        *p++ = matrix[0 + x][i + y];
    }

    for(i = 1; i < matrixSize; i++) {
        *p++ = matrix[i + x][matrixColSize - 1 + y];
    }

    --matrixSize;
    --matrixColSize;
    if(matrixSize == 0 || matrixColSize == 0) {
        return NULL;
    }

    for(i = matrixColSize - 1; i >= 0; i--) {
        *p++ = matrix[matrixSize + x][i + y];
    }
    for(i = matrixSize - 1; i > 0; i--) {
        *p++ = matrix[i + x][0 + y];
    }
    return p;
}

int* spiralOrder(int** matrix, int matrixSize, int* matrixColSize, int* returnSize){
    
    if(matrixSize == 0 || matrixColSize == 0) {
        *returnSize = 0;
        return NULL;
    }
    *returnSize = *matrixColSize * matrixSize;
    int *ret = malloc(sizeof(int) * (*returnSize));
    int *status = ret;
    int x = 0;
    int y = 0;
    do {
        status = build(matrix, matrixSize, *matrixColSize, &status, x, y);
        ++x; 
        ++y;
        matrixSize -= 2;
        *matrixColSize -= 2;
        if(matrixSize <= 0 || *matrixColSize <= 0)
            break;
    }while(status);
    return ret;
}



int main(int argc, char *argv[])
{
    return 0;
}

/**
 * @brief 螺旋矩阵, 分解螺旋成为一层一层的剥离;
 * ! 坑就是 matrix 变量转移的问题. &(&matrix[1][1]) 这种用法有问题! matrix指针初始问题.
 * 
 */
 
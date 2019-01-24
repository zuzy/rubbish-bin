#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INT_SIZE	(sizeof(int))

void rotate(int** matrix, int matrixRowSize, int *matrixColSizes) {
    printf("%d %d \n", matrixRowSize, *matrixColSizes);
    int *tmp = (int *)malloc(matrixRowSize * INT_SIZE);
    int size = matrixRowSize;
    int count = size;
    int index = 0;
    while(count > 1) {
        memcpy(tmp, matrix[index]+index, count * INT_SIZE);
        int i;
        printf("tmp is :");
        for(i = 0; i < count; i++) {
            printf("%d ", tmp[i]);
        }
        printf("\n");
        for(i = 0; i < count; i++) {
            matrix[index][index + count - 1 - i] = matrix[index + i][index];
        }
        for(i = 0; i < count; i++) {
            matrix[index + i][index] = matrix[index + count - 1][index + i];
        }
        for(i = 0; i < count; i++) {
            matrix[index + count - 1][index + i] = matrix[index + count - 1 - i][index + count - 1];
        }
        for(i = 0; i < count; i++) {
            matrix[index + count - 1 - i][index + count - 1] = tmp[count -1 -i];
        }

        count -= 2;
        ++index;
    }
}

int main(int argc, char *argv[])
{
    return 0;
}
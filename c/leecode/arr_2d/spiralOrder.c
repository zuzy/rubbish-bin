#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define print(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
struct position {
    int x;
    int y;
};

void _circle(int **matrix, struct position s, struct position e, int **input)
{
    int *p = *input;
    struct position pos;
    memcpy(&pos, &s, sizeof(pos));
    print("(%d, %d)", pos.y, pos.x);
    print("e:(%d, %d)", e.y, e.x);

    for(; pos.x < e.x; ++pos.x) {
        *p++ = matrix[pos.y][pos.x];
    }
    --pos.x;
    ++pos.y;
    print("(%d, %d)", pos.y, pos.x);
    if(pos.y >= e.y) {
        return;
    }
    for(; pos.y < e.y; ++pos.y) {
        *p++ = matrix[pos.y][pos.x];
    }
    --pos.x;
    --pos.y;
    print("(%d, %d)", pos.y, pos.x);
    for(; pos.x >= s.x; --pos.x) {
        *p++ = matrix[pos.y][pos.x];
    }
    --pos.y;
    ++pos.x;
    print("(%d, %d)", pos.y, pos.x);
    for(; pos.y > s.y && pos.x >= 0; --pos.y) {
        *p++ = matrix[pos.y][pos.x];
    }
}

int* spiralOrder(int** matrix, int matrixRowSize, int matrixColSize) {

    int i, j;
    for(i = 0; i < matrixRowSize; ++i) {
        for(j = 0; j < matrixColSize; ++j) {
            printf("(%d,%d) --> (%d)\n", i, j, matrix[i][j]);
        }
    }
    #if 0
    int i = matrixRowSize;
    int j = matrixColSize;
    #else 
    i = matrixColSize;
    j = matrixRowSize;
    #endif
    print("malloc %d", matrixColSize * matrixRowSize);

    int *ret = (int*)malloc(matrixColSize * matrixRowSize * sizeof(int));
    struct position start = {0, 0};
    struct position end = {i, j};
    int *p = ret;
    printf("sss\n");

    while(start.x <= end.x && start.y <= end.y) {
        _circle(matrix, start, end, &p);
        ++start.x;
        ++start.y;
        --end.x;
        --end.y;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int **s = (int **)calloc(10, sizeof(int *));
    int i = 0;
    int j = 0;
    for(i = 0; i < 10; i++) {
        s[i] = calloc(1, sizeof(int));
        for(j = 0; j < 1; j++) {
            s[i][j] = i+j;
            printf("(%d, %d) %d ", i, j, s[i][j]);
        }
        printf("\n");
    }
    
    printf("sdf\n");
    int len  = 10;
    int *ret = spiralOrder(s, 10, 1);
    for(i = 0; i < len; ++i) {
        printf("%d --> %d\n", i, ret[i]);
    }
    free(ret);
    return 0;
}
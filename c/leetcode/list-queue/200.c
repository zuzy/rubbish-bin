#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/**
 * 200. 岛屿数量
 * https://leetcode-cn.com/problems/number-of-islands/
 * 
 * */

#define COPY_EN

void bfs_island(char ***igrid, int row, int col, int s_r, int s_c)
{
    if(row < 0 || row >= s_r || col < 0 || col >= s_c) {
        return;
    }
    char **grid = *igrid;
    if(grid[row][col] == '1') {
        // printf("(%d, %d) reset to zero\n", row, col);
        grid[row][col] = '0';
        bfs_island(igrid, row - 1, col, s_r, s_c);
        bfs_island(igrid, row, col - 1, s_r, s_c);
        bfs_island(igrid, row + 1, col, s_r, s_c);
        bfs_island(igrid, row, col + 1, s_r, s_c);
    }
}


int numIslands(char** igrid, int gridSize, int* gridColSize){
    if(igrid == NULL || gridColSize == NULL || gridSize == 0 || *gridColSize == 0) {
        return 0;
    }
#if COPY_EN
    char **grid = igrid;
#else
    char **grid = malloc(gridSize * sizeof(char*));
    int k;
    for(k = 0; k < gridSize; ++k) {
        grid[k] = malloc(*gridColSize);
        memcpy(grid[k],igrid[k], *gridColSize);
    }
#endif
    

    printf("gridSize = %d\n", gridSize);
    printf("gridColSize = %d\n", *gridColSize);
    int ret = 0;
    int i, j;
    for(i = 0; i < gridSize; i++) {
        for(j = 0; j < *gridColSize; j++) {
            printf("%c \t", grid[i][j]);
            if(grid[i][j] == '1') {
                ++ret;
                bfs_island(&grid, i, j, gridSize, *gridColSize);
            }
        }
        printf("\n");
    }
#if COPY_EN
#else
    for(k = 0; k < gridSize; ++k) {
        free(grid[k]);
    }
    free(grid);
#endif

    return ret;
}


int main(int argc, char** argv)
{
    char *a[] = {
        "11000",
        "11000",
        "00100",
        "00011",
    };
    int col = 5;
    int ret = numIslands(a, 4, &col);
    printf("ret is %d\n", ret);

    return 0;
}
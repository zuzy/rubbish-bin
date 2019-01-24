#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_loc {
    int x;
    int y;
} loc;

int check_island(int *des, int **grid, int size, int colsize, loc addr)
{
    
    if(addr.x >= size || addr.y >= colsize || addr.x < 0 || addr.y < 0) {
        return 1;
    }
    if(*des == 0) {
        printf("check (%d, %d) tmp == 0\n", addr.x, addr.y);
        int i, j;
        for(i = 0; i < size; i++) {
            for(j = 0; j < colsize; j++) {
                if(grid[i][j]) {
                    addr.x = i;
                    addr.y = j;
                    printf("get new (%d, %d)\n", i, j);
                    printf("%d (%d, %d)\n", *des, addr.x, addr.y);
                    goto End;
                }
            }
        }
        printf("empty !!!!!!!\n");
        return 0;
    }
    End:
    printf("check (%d, %d)\n", addr.x, addr.y);
    if(grid[addr.x][addr.y]) {
        ++*des;
        grid[addr.x][addr.y] = 0;
        printf("check ok! %d (%d, %d)\n", *des, addr.x, addr.y);
        loc left;
        left.x = addr.x - 1;
        left.y = addr.y;
        check_island(des, grid, size, colsize, left);
        loc up;
        up.x = addr.x;
        up.y = addr.y - 1;
        check_island(des, grid, size, colsize, up);
        loc right;
        right.x = addr.x + 1;
        right.y = addr.y;
        check_island(des, grid, size, colsize, right);
        loc down;
        down.x = addr.x;
        down.y = addr.y + 1;
        check_island(des, grid, size, colsize, down);
    }
    return 2;
}

int maxAreaOfIsland(int** grid, int gridRowSize, int gridColSize) {
    int max, tmp;
    max = 0;
    tmp = 0;
    loc a = {
        .x = 0,
        .y = 0,
    };
    int ret = 0;
    int i = 0;
    do{
        ret = check_island(&tmp, grid, gridRowSize, gridColSize,  a);
        printf("get area %d\n ret = %d\n", tmp, ret);
        if(ret == 2) {
            if(tmp > max) {
                max = tmp;
            }
        }
        tmp = 0;
    }while(ret);

    return max;

    
}
#if 0
static int res[5][5] = {
    {0,1,1,1,0},
    {1,1,1,1,0},
    {0,0,1,1,0},
    {1,1,0,1,0},
    {0,1,1,0,1},
};
#endif
static int res[5][5] = {
    {1,0,0,0,0},
    {0,0,0,0,0},
    {0,0,1,1,0},
    {0,0,0,0,0},
    {0,0,0,0,1},
};

int main(int argc, char *argv[])
{
    int ** des = calloc(5, sizeof(int *));
    int **p = des;
    int i, j;
    for(i = 0; i < 5; i++) {
        *p = calloc(5, sizeof(int));
        memcpy(*p, res[i], 5 * sizeof(int));
        ++p;
    }
    for(i = 0; i < 5; i++) {
        for(j = 0; j < 5; j++) 
            printf("%d ", des[i][j]);
        printf("\n");
    }

    int ret = maxAreaOfIsland(des, 5, 5);
    printf("get ret %d\n", ret);

    for(i = 0; i < 5; i++) {
        for(j = 0; j < 5; j++) 
            printf("%d ", des[i][j]);
        printf("\n");
    }

    return 0;
}
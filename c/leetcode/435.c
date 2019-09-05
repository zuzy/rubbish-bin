#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * https://leetcode-cn.com/problems/non-overlapping-intervals/
 * 435. 无重叠区间
 * */


typedef struct st_range_list {
    int dummy;
    int start;
    int end;
    int span_num;
    struct st_range_list *next;
    struct st_range_list *prev;
} range_list_t;

void _dump_intervals(int **intervals, int intervalsSize, int *intervalsColSize)
{
    int i, j;
    for(i = 0; i < intervalsSize; ++i) {
        printf("index: %d: (", i);
        for(j = 0; j < *intervalsColSize; ++j) {
            if(j)
                printf(", %d", intervals[i][j]);
            else 
                printf("%d", intervals[i][j]);
        }
        printf(")\n");
    }
}

void _add_to_range_list(range_list_t *dummy, int start, int end)
{
    range_list_t *ptr = dummy->next;
    while(ptr->next != dummy) {
        
    }
}

int build_range_list(range_list_t *dummy, int **intervals, int intervalsSize, int *intervalsColSize)
{
    int i; 
    for(i = 0; i < intervalsSize; ++i) {

    }
    return 0;
}

int eraseOverlapIntervals(int** intervals, int intervalsSize, int* intervalsColSize){
    _dump_intervals(intervals, intervalsSize, intervalsColSize);

    // add to a list!

    return 0;
}


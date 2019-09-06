#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * https://leetcode-cn.com/problems/non-overlapping-intervals/
 * 435. 无重叠区间
 * */


typedef struct st_range_list {
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

void _dump_range_list(range_list_t *dummy)
{
    range_list_t *ptr = dummy->next;
    int index = 0;
    printf("dump\n");
    while(ptr != dummy) {
        printf("\t%d -> span: %d (%d, %d)\n", index++, ptr->span_num, ptr->start, ptr->end);
        ptr = ptr->next;
    }
}

void _clean_range_list(range_list_t *dummy)
{
    range_list_t *ptr = dummy->next;
    while (ptr != dummy) {
        range_list_t *tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
}

void _add_to_range_list(range_list_t *dummy, int start, int end)
{
    range_list_t *ptr = dummy->next;
    while(ptr != dummy) {
        if(ptr->start == start) {
            if(end < ptr->end) {
                ptr->end = end;
            }
            return;
        }
        if(start < ptr->start) {
            break;
        }
        ptr = ptr->next;
    }
    range_list_t *add = calloc(1, sizeof(range_list_t));
    add->start = start;
    add->end = end;

    add->next = ptr;
    add->prev = ptr->prev;
    ptr->prev->next = add;
    ptr->prev = add;
    
}

int build_range_list(range_list_t *dummy, int **intervals, int intervalsSize, int *intervalsColSize)
{
    int i; 
    for(i = 0; i < intervalsSize; ++i) {
        _add_to_range_list(dummy, intervals[i][0], intervals[i][1]);
    }
    return 0;
}

int get_span_range_list(range_list_t *dummy)
{
    int max_span = 0;
    range_list_t *ptr, *tmp;
    ptr = dummy->next;
    while(ptr != dummy) {
        tmp = ptr->prev;
        while(tmp != dummy && tmp->end > ptr->start) {
            tmp = tmp->prev;
        }
        ptr->span_num = tmp->span_num + 1;
        if(ptr->span_num > max_span) {
            max_span = ptr->span_num;
        }
        ptr = ptr->next;
    }
    return max_span;
}

int eraseOverlapIntervals(int** intervals, int intervalsSize, int* intervalsColSize){
    // _dump_intervals(intervals, intervalsSize, intervalsColSize);
    
    range_list_t dummy = {.next = &dummy, .prev = &dummy};
    build_range_list(&dummy, intervals, intervalsSize, intervalsColSize);
    int max_span = get_span_range_list(&dummy);
    // _dump_range_list(&dummy);
    _clean_range_list(&dummy);
    return intervalsSize - max_span;
}

[[0,2],[1,3],[2,4],[3,5],[4,6]]
[[1,100],[11,22],[1,11],[2,12]]

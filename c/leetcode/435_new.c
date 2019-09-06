#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * https://leetcode-cn.com/problems/non-overlapping-intervals/
 * 435. 无重叠区间
 * */

void _dump_intervals(int **intervals, int intervalsSize, int *intervalsColSize)
{
    int i, j;
    printf("dump\n");
    for(i = 0; i < intervalsSize; ++i) {
        printf("\tindex: %d: (", i);
        for(j = 0; j < *intervalsColSize; ++j) {
            if(j)
                printf(", %d", intervals[i][j]);
            else 
                printf("%d", intervals[i][j]);
        }
        printf(")\n");
    }
}

int get_span_range_list(int **intervals, int intervalsSize, int *intervalsColSize)
{
    int max_span = 0;
    int i, j;
    int span_list[intervalsSize];
    for(i = 0; i < intervalsSize; i++) {
        span_list[i] = 1;
        for(j = i; j >= 0; j--) {
            if(intervals[j][0] < intervals[i][0] && intervals[j][1] <= intervals[i][0]) {
                printf("get! (%d, %d)\n", intervals[j][0], intervals[j][1]);
                span_list[i] = span_list[j] + 1;
                break;
            }
        }
        printf("get span %d\n", span_list[i]);
        if(max_span < span_list[i]) {
            max_span = span_list[i];
        }
    }
    return max_span;
}

int _comp_(const void *a, const void *b)
{
    int *aa = *(int **)a;
    int *bb = *(int **)b;

    if(aa[0] == bb[0]) {
        return (aa[1] > bb[1]) - (aa[1] < bb[1]);
    }
    return (aa[0] > bb[0]) - (aa[0] < bb[0]);
}

int eraseOverlapIntervals(int** intervals, int intervalsSize, int* intervalsColSize){
    if(!intervalsSize) return 0;
    _dump_intervals(intervals, intervalsSize, intervalsColSize);
    qsort(intervals, intervalsSize, sizeof(int*), _comp_);
    _dump_intervals(intervals, intervalsSize, intervalsColSize);

    // build_range_list(intervals, intervalsSize, intervalsColSize);
    int max_span = get_span_range_list(intervals, intervalsSize, intervalsColSize);

    return intervalsSize - max_span;
}


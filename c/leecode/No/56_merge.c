#include <stdlib.h>
#include <stdio.h>

struct Interval {
    int start;
    int end;
};

/**
 * Definition for an interval.
 * struct Interval {
 *     int start;
 *     int end;
 * };
 */
/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
struct Interval* merge(struct Interval* intervals, int intervalsSize, int* returnSize) {
    struct Interval *ptr = intervals;
    int i, j;
    for(i = 0; i < intervalsSize; i++) {
        if(intervals[i].start < intervals[i].end) {
            continue;
        }
        int s_i = intervals[i].start;
        int e_i = intervals[i].end;
        for(j = i; j < intervalsSize; j++) {
            if(intervals[j].start < intervals[i].end) {
                continue;
            }
            int s_j = intervals[j].start;
            int e_j = intervals[j].end;
            if(s_i <= s_j && e_i >= e_j) {
                intervals[j].end = intervals[j].start - 1;
                continue;
            }
            if(s_i <= e_j && e_i >= e_j) {
                
            }
        }
    }
}
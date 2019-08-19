#include <stdio.h>
#include <stdlib.h>

/**
 * @brief https://leetcode-cn.com/problems/candy/
 * 
 */

typedef struct st_candy{
    int head;
    int *p_head;
    int tail;
    int *p_tail;
    int *min;
}candy_t;

static int *_min_of_list(int *ratings, int ratingsSize)
{
    int *min = ratings;
    int i;
    for(i = 0; i < ratingsSize; i++) {
        if(ratings[i] < *min) {
            min = &ratings[i];
        }
    }
    return min;
}

int candy(int* ratings, int ratingsSize)
{
    int *min = _min_of_list(ratings, ratingsSize);
    int *prev, *next;
    prev = min - 1;
    next = min + 1;
    int apples = 1;
    while(prev > ratings) {
        if(prev - )
    }
    while(next - ratings < ratingsSize) {

    }

    return 0;
}


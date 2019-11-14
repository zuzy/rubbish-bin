#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * 134. 加油站
 * https://leetcode-cn.com/problems/gas-station/
 * */

static int _checkout_route(int *route, int size, int start)
{
    int *pos = route + start;
    int i = start;
    int sum = 0;
    for(; i < size; i++) {
        sum += route[i];
        if(sum < 0) {
            return -1;
        }
    }
    for(i = 0; i < start; i++) {
        sum += route[i];
        if(sum < 0) {
            return -1;
        }
    }
    return 0;
}

int canCompleteCircuit(int* gas, int gasSize, int* cost, int costSize)
{
    int diff[gasSize];
    int i;
    int sum = 0;
    for(i = 0; i < gasSize; i++) {
        diff[i] = gas[i] - cost[i];
        sum += diff[i];
    }
    if(sum < 0) {
        return -1;
    }
    for(i = 0; i < gasSize; i++) {
        if(diff[i] < 0) continue;
        if(_checkout_route(diff, gasSize, i) == 0) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int gas[]  = {1,2,3,4,5};
    int cost[] = {3,4,5,1,2};
    int ret = canCompleteCircuit(gas, 5, cost, 5);
    printf("ret is %d\n", ret);
    return 0;
}

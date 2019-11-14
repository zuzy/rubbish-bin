#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bool.h>



typedef struct {
    int *list;
    int start;
    int end;
    int length;
    int full;
    int empty;
} MyCircularQueue;

/** Initialize your data structure here. Set the size of the queue to be k. */

MyCircularQueue* myCircularQueueCreate(int k) {
    MyCircularQueue *ret = calloc(1, sizeof(MyCircularQueue));
    if(ret) {
        ret->list = malloc(k * sizeof(int));
        if(!ret->list) {
            free(ret);
            return NULL;
        }
        ret->empty = 1;
        ret->length = k;
    }
    return ret;
}

/** Insert an element into the circular queue. Return true if the operation is successful. */
bool myCircularQueueEnQueue(MyCircularQueue* obj, int value) {
    if(obj->full) return false;
    obj->list[obj->end++] = value;
    if(obj->end == obj->length) {
        obj->end = 0;
    }
    if(obj->end == obj->start) {
        obj->full = 1;
    }
    obj->empty = 0;
    return true;
}

/** Delete an element from the circular queue. Return true if the operation is successful. */
bool myCircularQueueDeQueue(MyCircularQueue* obj) {
    if(obj->empty) return false;
    ++obj->start;
    if(obj->start == obj->length) {
        obj->start = 0;
    }
    if(obj->start == obj->end) {
        obj->empty = 1;
    }
    obj->full = 0;
    return true;
}

/** Get the front item from the queue. */
int myCircularQueueFront(MyCircularQueue* obj) {
    if(obj->empty) return -1;
    return obj->list[obj->start];
}

/** Get the last item from the queue. */
int myCircularQueueRear(MyCircularQueue* obj) {
    if(obj->empty) return -1;
    if(obj->end) {
        return obj->list[obj->end -1];
    } else {
        return obj->list[obj->length - 1];
    }
}

/** Checks whether the circular queue is empty or not. */
bool myCircularQueueIsEmpty(MyCircularQueue* obj) {
    return obj->empty;
}

/** Checks whether the circular queue is full or not. */
bool myCircularQueueIsFull(MyCircularQueue* obj) {
    return obj->full;
}

void myCircularQueueFree(MyCircularQueue* obj) {
    if(obj) {
        if(obj->list) {
            free(obj->list);
        }
        free(obj);
    }
}

/**
 * Your MyCircularQueue struct will be instantiated and called as such:
 * MyCircularQueue* obj = myCircularQueueCreate(k);
 * bool param_1 = myCircularQueueEnQueue(obj, value);
 
 * bool param_2 = myCircularQueueDeQueue(obj);
 
 * int param_3 = myCircularQueueFront(obj);
 
 * int param_4 = myCircularQueueRear(obj);
 
 * bool param_5 = myCircularQueueIsEmpty(obj);
 
 * bool param_6 = myCircularQueueIsFull(obj);
 
 * myCircularQueueFree(obj);
*/
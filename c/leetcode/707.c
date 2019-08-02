#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int value;
    void *prev;
    void *next;    
} MyLinkedList;

/** Initialize your data structure here. */

MyLinkedList* myLinkedListCreate() {
    MyLinkedList *ret = malloc(sizeof(MyLinkedList));
    if(ret) {
        ret->next = ret->prev = ret;
    }
    return ret;
}

/** Get the value of the index-th node in the linked list. If the index is invalid, return -1. */
int myLinkedListGet(MyLinkedList* obj, int index) {
    
    if(index < 0 || !obj) {
        return -1;
    }

    int i = 0;
    MyLinkedList *tmp = obj;
    do {
        tmp = tmp->next;
        if(tmp == obj) {
            return -1;
        }
    } while(i++ < index);
    return tmp->value;
}

/** Add a node of value val before the first element of the linked list. After the insertion, the new node will be the first node of the linked list. */
void myLinkedListAddAtHead(MyLinkedList* obj, int val) {
    MyLinkedList *tmp = (MyLinkedList *)malloc(sizeof(MyLinkedList));
    if(!tmp) {
        printf("Create new node failed\n");
        return;
    }
    tmp->value = val;
    ((MyLinkedList *)obj->next)->prev = tmp;
    tmp->next = obj->next;
    tmp->prev = obj;
    obj->next = tmp;
}

/** Append a node of value val to the last element of the linked list. */
void myLinkedListAddAtTail(MyLinkedList* obj, int val) {
    MyLinkedList *tmp = (MyLinkedList *)malloc(sizeof(MyLinkedList));
    if(!tmp) {
        printf("Create new node failed\n");
        return;
    }
    tmp->value = val;
    ((MyLinkedList *)obj->prev)->next = tmp;
    tmp->prev = obj->prev;
    obj->prev = tmp;
    tmp->next = obj;
}

/** Add a node of value val before the index-th node in the linked list. If index equals to the length of linked list, the node will be appended to the end of linked list. If index is greater than the length, the node will not be inserted. */
void myLinkedListAddAtIndex(MyLinkedList* obj, int index, int val) {
    if(!obj) {
        return;
    }

    if(index < 0) {
        myLinkedListAddAtTail(obj, val);
        return;
    }

    int i = 0;
    MyLinkedList *tmp = obj;
    for(; i < index; i++) {
        tmp = tmp->next;
        if(tmp == obj) {
            return;
        }
    }

    MyLinkedList *add = (MyLinkedList *)malloc(sizeof(MyLinkedList));
    if(add) {
        add->value = val;
        ((MyLinkedList *)tmp->next)->prev = add;
        add->next = tmp->next;
        add->prev = tmp;
        tmp->next = add;
    }
}

/** Delete the index-th node in the linked list, if the index is valid. */
void myLinkedListDeleteAtIndex(MyLinkedList* obj, int index) {
    if(index < 0 || !obj) {
        return;
    }

    int i = 0;
    MyLinkedList *tmp = obj;
    do {
        tmp = tmp->next;
        if(tmp == obj) {
            return;
        }
    }while(i++ < index);

    ((MyLinkedList *)tmp->prev)->next = tmp->next;
    ((MyLinkedList *)tmp->next)->prev = tmp->prev;
    free(tmp);
}

void myLinkedListFree(MyLinkedList* obj) {
    MyLinkedList *tmp = obj->next;
    for(;;) {
        if(tmp == obj) {
            break;
        }
        MyLinkedList *del = tmp;
        tmp = tmp->next;
        free(del);
    }
    free(obj);
}

/**
 * Your MyLinkedList struct will be instantiated and called as such:
 * MyLinkedList* obj = myLinkedListCreate();
 * int param_1 = myLinkedListGet(obj, index);
 
 * myLinkedListAddAtHead(obj, val);
 
 * myLinkedListAddAtTail(obj, val);
 
 * myLinkedListAddAtIndex(obj, index, val);
 
 * myLinkedListDeleteAtIndex(obj, index);
 
 * myLinkedListFree(obj);
*/


int main(int argc, char *argv[])
{
    return 0;
}
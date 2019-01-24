#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode *reverseList(struct ListNode *head)
{
    if(!head || !head->next) return head;
    struct ListNode dummy, *prev, *next, *p;
    prev = &dummy;
    prev->next = NULL;
    p = head;
    while(p) {
        next = p->next;
        p->next = prev;
        prev = p;
        p = next;
    }
    head->next = NULL;
    return prev;
}

struct ListNode* reverseBetween(struct ListNode* head, int m, int n) {
    struct ListNode *prev, d, *p, *r, *next;
    prev = &d;
    prev->next = head;
    if(m > 1) 
        r = head;
    else 
        r = NULL;

    int i = 1;
    for(; i < m; i++) {
        prev = prev->next;
    }
    p = prev->next;
    for(; i < n; i++) {
        next = p->next;
        p->next = prev;
        prev = p;
        p = next;
    }
    return r? :prev;
}

int main(int argc, char *argv[])
{
    return 0;
}
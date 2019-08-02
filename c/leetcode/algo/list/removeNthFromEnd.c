#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

 struct ListNode {
     int val;
     struct ListNode *next;
 };

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* removeNthFromEnd(struct ListNode* head, int n) {
    int size = 1;
    struct ListNode *p = head;
    while(p->next) {
        p = p->next;
        ++size;
    }
    int i, len;
    for(p = head, i = 0, len = size-n; i < len; ++i)
        p = p->next;
    if(p->next) {
        p->val = p->next->val;
        struct ListNode *tmp = p->next;
        p->next = p->next->next;
        free(tmp);
    } else {
        free(p);
        p = NULL;
    }
}

static struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
    if (n < 1) return NULL;

    struct ListNode *p, *prev, dummy;
    dummy.next = head;
    p = prev = &dummy;
    while (n-- > 0) {
        p = p->next;
    }

    while (p->next != NULL) {
        p = p->next;
        prev = prev->next;
    }

    struct ListNode *tmp = prev->next;
    prev->next = tmp->next;
    if (tmp == head) {
        head = tmp->next;
    }
    return head;
}

int main(int argc, char *argv[]) 
{
    return 0;
}
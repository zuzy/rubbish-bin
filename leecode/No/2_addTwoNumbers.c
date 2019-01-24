#include <stdio.h>
#include <stdlib.h>

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
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode *p1, *p2, *ret, *pr, *pp;
    p1 = l1;
    p2 = l2;
    ret = malloc(sizeof(struct ListNode));
    ret->val = 0;
    ret->next = NULL;
    pr = ret;
    int en = 0;
    while(p1 || p2) {
        if(p2) {
            pr->val += p2->val;
            p2 = p2->next;
        }
        if(p1) {
            pr->val += p1->val;
            p1 = p1->next;
        }
        if(pr->val > 9) {
            pr->val -= 10;
            en = 1;
        } else {
            en = 0;
        }
        struct ListNode *t = malloc(sizeof(struct ListNode));
        t->val = en;
        t->next = NULL;
        pr->next = t;
        pp = pr;
        pr = t;
    }
    if(!pr->val) {
        pp->next = NULL;
        free(pr);
    }

    return ret;
}

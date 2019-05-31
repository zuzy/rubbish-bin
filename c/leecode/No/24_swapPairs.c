#include <stdlib.h>
#include <stdio.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode* swapPairs(struct ListNode* head) {
    struct ListNode *p, *pre, dummy;
    pre = &dummy;
    pre->next = p = head;
    while(p) {
        struct ListNode *p1, *p2;
        p1 = p->next;
        if(p1 == NULL) {
            break;
        }
        p2 = p1->next;
        
        pre->next = p1;
        p1->next = p;
        p->next = p2;

        pre = p;
        p = p->next;
    }
    return dummy.next;
}

int main(int argc,char *argv[])
{
    return 0;
}
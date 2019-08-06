#include <stdio.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

static void _dump_list(struct ListNode *head)
{
    struct ListNode *p = head;
    printf("start: begin");
    while(p) {
        printf(" -> %d", p->val);
        p = p->next;
    }
    printf(" [end]\n");
}

#define _dump_list(head) NULL

struct ListNode* partition(struct ListNode* head, int x)
{
    struct ListNode *ptr = head;
    struct ListNode dummy = {0, head};
    struct ListNode *prev = &dummy;
    struct ListNode *tmp, tmp_dummy;
    tmp = &tmp_dummy;

    while(ptr != NULL) {
        if(ptr->val >= x) {
            tmp->next = ptr;
            tmp = tmp->next;

            prev->next = ptr->next;
            printf(">=: ");
            _dump_list(tmp_head);
        } else {
            prev = prev->next;
            _dump_list(head);
        }
        ptr = ptr->next;
    }

    tmp->next = NULL;
    prev->next = tmp_dummy.next;

    return dummy.next;
}


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
#if 1
static void fun(struct ListNode *p, struct ListNode *head)
{
    if(p == NULL) return;
    if(p->next == NULL) {
        head->next = p;
        return;
    }
    fun(p->next, head);
    p->next->next = p;
    p->next = NULL;
}

struct ListNode* reverseList(struct ListNode* head) {
    if(head && head->next)
        fun(head->next, head);
    return head;
}
#else
struct ListNode* reverseList(struct ListNode* head) {
    struct ListNode *ptr, *ret, dummy;
    ret = NULL;
    ptr = head;
    while(ptr) {
        struct ListNode *tmp = (struct ListNode*)malloc(sizeof(struct ListNode));
        tmp->val = ptr->val;
        tmp->next = ret;
        ret = tmp;
        ptr = ptr->next;
    }
    return ret;
}
#endif
int main(int argc, char *argv[]) 
{
    return 0;
}
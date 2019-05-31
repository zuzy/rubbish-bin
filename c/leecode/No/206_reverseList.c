#include <stdlib.h>
#include <stdio.h>
struct ListNode {
    int val;
    struct ListNode *next;
};
#if 0
// static struct ListNode *tmp = NULL;
struct ListNode *reverse(struct ListNode *head, struct ListNode **ret)
{
    if(head->next == NULL) {
        *ret = head;
        return head;
    }
    reverse(head->next, ret)->next = head;
    head->next = NULL;
    return head;
}
struct ListNode *reverseList(struct ListNode *head)
{
    if(!head || !head->next) return head;
    struct ListNode *tmp = NULL;
    reverse(head, &tmp);
    return tmp;
}
#else
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
        // printf("p->%d\n", p->val);
    }
    head->next = NULL;
    return prev;
}
#endif

int main(int argc, char *argv[])
{
    struct ListNode *head = calloc(1, sizeof(struct ListNode));
    int i = 1;
    struct ListNode *ptr = head;
    for(; i < 10; i++) {
        struct ListNode *t = calloc(1, sizeof(struct ListNode));
        ptr->next = t;
        t->val = i;
        ptr = t;
    }
    ptr = head;
    while(ptr){
        printf("ptr -> %d\n", ptr->val);
        ptr = ptr->next;
    }
    printf("---------\n");
    struct ListNode *a = reverseList(head);
    ptr = a;
    while(ptr){
        printf("ptr -> %d\n", ptr->val);
        ptr = ptr->next;
    }
    return 0;
}
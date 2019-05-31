#include <stdlib.h>
#include <stdio.h>
struct ListNode {
    int val;
    struct ListNode *next;
};

// static struct ListNode *tmp = NULL;
struct ListNode *reverse(struct ListNode *head, struct ListNode **ret, int index)
{
    // if(head->next == NULL) {
    if(index == 0) {
        *ret = head;
        return head;
    }
    printf("r %d\n", index);
    reverse(head->next, ret, index - 1)->next = head;
    head->next = NULL;
    return head;
}
struct ListNode *reverseList(struct ListNode *head, int index)
{
    if(!head || !head->next) return head;
    static struct ListNode *tmp = NULL;
    reverse(head, &tmp, index);
    return tmp;
}

struct ListNode* reverseKGroup(struct ListNode* head, int k) {
    struct ListNode *ret,*h, *e, *next, d, *prev;
    prev = &d;
    ret = NULL;
    prev->next = h = e = head;
    
    do {
        int i;
        for(i = 0; i < k; i++) {
            printf("%d -> %d\n", i, e->val);
            e = e->next;
            if(!e) {
                return ret? :head;
            }
        }
        next = reverseList(h, k-1);
        prev->next = next;
        prev = h;
        h->next = e;
        h = e;
        if(ret == NULL) {
            ret = next;
        }
        printf("next->val %d\n", next->val);
    } while(1);
    
}

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
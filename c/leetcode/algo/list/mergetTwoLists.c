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
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* mergeTwoLists(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode *tmp, dummy;
    dummy.next = NULL;
    tmp = &dummy;

    while(l1 || l2) {
        tmp->next = (struct ListNode*)malloc(sizeof(struct ListNode));
        tmp = tmp->next;
        tmp->next = NULL;
        if(l1 && l2) {
            if(l1->val < l2->val) {
                tmp->val = l1->val;
                l1 = l1->next;
            } else {
                tmp->val = l2->val;
                l2 = l2->next;
            }
        } else if(l1) {
            tmp->val = l1->val;
            l1 = l1->next;
        } else {
            tmp->val = l2->val;
            l2 = l2->next;
        }
    }
    return dummy.next;
}

int main(int argc, char *argv[]) 
{
    return 0;
}
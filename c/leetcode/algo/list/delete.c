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
void deleteNode(struct ListNode* node) {
    node->val = node->next->val;
    struct ListNode *tmp = node->next;
    node->next = node->next->next;
    free(tmp);
}

int main(int argc, char *argv[]) 
{
    return 0;
}
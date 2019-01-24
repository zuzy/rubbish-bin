#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

/**
 * @brief 最慢的方法...
 * 
 */

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* mergeKLists(struct ListNode** lists, int listsSize) {
    struct ListNode dummy;
    dummy.next = NULL;
    printf("get size %d\n", listsSize);
    if(!listsSize) {
        return NULL;
    }
    int en;
    int min = 0;
    // struct ListNode *ret = calloc(1, sizeof(*ret));
    struct ListNode *p = &dummy;
    while(1) {
        int i;
        en = 0;
        min = -1;
        for(i = 0; i < listsSize; i++) {
            if(lists[i]) {
                if(min < 0) {
                    min = i;
                } else {
                    if(lists[min]->val > lists[i]->val) {
                        min = i;
                    }
                }
                en = 1;
            }
        }
        if(en) {
            struct ListNode *node = calloc(1, sizeof(*node));
            node->val = lists[min]->val;
            p->next = node;
            p = p->next;
            lists[min] = lists[min]->next;
        } else {
            break;
        }
    }
    return dummy.next;
}

int main(int argc, char *argv[])
{

}
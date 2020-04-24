/*
 * @lc app=leetcode.cn id=328 lang=cpp
 *
 * [328] 奇偶链表
 */
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

// @lc code=start
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */

class Solution {
public:
    ListNode* oddEvenList(ListNode* head) 
    {
        if(!head || !head->next) return head;
        bool is_even = true;
        ListNode *p, *odd, *even, *odd_head, *tmp;
        even = head;
        odd_head = odd = even->next;
        p = odd->next;
        while(p) {
            tmp = p->next;
            if(is_even) {
                even->next = p;
                even = even->next;
            } else {
                odd->next = p;
                odd = odd->next;
            }
            is_even = !is_even;
            p = tmp;
        }
        even->next = odd_head;
        tmp = head;
        odd->next = NULL;
        return head;
    }
};
// @lc code=end


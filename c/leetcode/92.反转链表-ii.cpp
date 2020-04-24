/*
 * @lc app=leetcode.cn id=92 lang=cpp
 *
 * [92] 反转链表 II
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
    ListNode* reverseBetween(ListNode* head, int m, int n) 
    {
        if(!head || !head->next) return head;
        ListNode dummy(0);
        dummy.next = head;
        ListNode *prev, *curr, *tmp;
        int i = 1;
        prev = &dummy;
        curr = dummy.next;
        for(; i < m; i++) {
            prev = prev->next;
        }
        curr = prev->next;
        ListNode *pprev = prev;
        ListNode *tail = curr;

        for(; i <= n; i++) {
            tmp = curr->next;
            curr->next = prev;
            prev = curr;
            curr = tmp;
        }
        tail->next = tmp;
        pprev->next = prev;
        return dummy.next;
    }
};
// @lc code=end


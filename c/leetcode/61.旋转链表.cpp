/*
 * @lc app=leetcode.cn id=61 lang=cpp
 *
 * [61] 旋转链表
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
    int get_size(ListNode *head) 
    {
        if(!head) return 0;

        int size = 0;
        ListNode dummy(0);
        dummy.next = head;
        ListNode *pre, *cur;
        pre = &dummy;
        cur = head;
        while(cur) {
            ++size;
            pre = pre->next;
            cur = cur->next;
        }

        pre ->next = head;
        return size;
    }
    
public:
    ListNode* rotateRight(ListNode* head, int k) 
    {
        if(!head || !head->next) return head;
        int size = get_size(head);
        k = size - k % size;
        for(int i = 0; i < k; ++i) {
            head = head->next;
        }
        ListNode *p = head;
        for(int i = 1; i < size; ++i) {
            p = p->next;
        }
        p->next = NULL;
        return head;
    }
};
// @lc code=end


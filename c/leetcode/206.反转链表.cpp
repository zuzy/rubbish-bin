/*
 * @lc app=leetcode.cn id=206 lang=cpp
 *
 * [206] 反转链表
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

    ListNode *helper(ListNode *p, ListNode *pn) 
    {
        if(!pn) {
            return p;
        }
        // cout << p->val << ": " << pn->val << endl;
        ListNode *next = pn->next;
        pn->next = p;
        return helper(pn, next);
    }

    ListNode* reverseList(ListNode* head) 
    {
        #if 1
        if(!head) return head;
        ListNode *n = head->next;
        if(!n) return head;
        head->next = NULL;
        return helper(head, n);
        #else
        ListNode *p, *pn, *t;
        p = head;
        if(!p) return p;
        pn = p->next;
        if(!pn) return p;

        p->next = NULL;
        while(pn) {
            t = pn->next;
            pn->next = p;
            p = pn;
            pn = t;
        }
        return p;
        #endif
    }
};
// @lc code=end


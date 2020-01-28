/*
 * @lc app=leetcode.cn id=203 lang=cpp
 *
 * [203] 移除链表元素
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
    ListNode* removeElements(ListNode* head, int val) 
    {
        if(!head) return head;
        ListNode dummy(0);
        dummy.next = head;
        ListNode *prev = &dummy;
        ListNode *curr = dummy.next;
        while(curr) {
            // cout << "(" << prev->val << ", " << curr->val << ")" << endl;
            if(curr->val == val) {
                prev->next = curr->next;
                ListNode *t = curr;
                curr = curr->next;
                delete t;
            } else {
                prev = prev->next;
                curr = curr->next;
            }
        }
        return dummy.next;
    }
};
// @lc code=end


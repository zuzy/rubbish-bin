/*
 * @lc app=leetcode.cn id=234 lang=cpp
 *
 * [234] 回文链表
 */
#include <iostream>
#include <stack>
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
    bool isPalindrome(ListNode* head) 
    {
        if(!head || !head->next) return true;
        stack<int> num_stack;
        ListNode *mid, *tail;
        mid = tail = head;
        bool even = false;
        while(tail) {
            if(even) {
                num_stack.push(mid->val);
                mid = mid->next;
            }
            tail = tail->next;
            even = !even;
        }
        
        if(even) {
            mid = mid->next;
        }

        int num;
        while(mid) {
            num = num_stack.top();
            num_stack.pop();
            if(num != mid->val) {
                return false;
            }
            mid = mid->next;
        }
        return true;
    }
};
// @lc code=end


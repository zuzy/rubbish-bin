/*
 * @lc app=leetcode.cn id=2 lang=cpp
 *
 * [2] 两数相加
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
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        int carry = 0;
        ListNode dummy(0), *p;
        p = &dummy;
        while(l1 && l2) {
            int sum = l1->val + l2->val + carry;
            if(sum > 9) {
                carry = 1;
                sum -= 10;
            } else {
                carry = 0;
            }
            ListNode *t = new ListNode(sum);
            p->next = t;
            p = p->next;
            l1 = l1->next;
            l2 = l2->next;
        }
        if(l1 || l2) {
            ListNode *l = l1 ? l1 : l2;
            while(l) {
                int sum = l->val + carry;
                if(sum > 9) {
                    carry = 1;
                    sum -= 10;
                } else {
                    carry = 0;
                }
                p->next = new ListNode(sum);
                l = l->next;
                p = p->next;
            }
        }
        if(carry) {
            p->next = new ListNode(1);
        }
        return dummy.next;
    }
};
// @lc code=end


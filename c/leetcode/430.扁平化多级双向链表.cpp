/*
 * @lc app=leetcode.cn id=430 lang=cpp
 *
 * [430] 扁平化多级双向链表
 */
#include <iostream>
using namespace std;
class Node {
public:
    int val;
    Node* prev;
    Node* next;
    Node* child;
};
// @lc code=start
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* prev;
    Node* next;
    Node* child;
};
*/
class Solution {
public:
    Node* flatten_helper(Node* head) 
    {
        Node *chead, *ctail, *p, *child;
        p = head;
        while(p->next) {
            if(p->child) {
                break;
            }
            p = p->next;
        }
        ctail = p;
        if(p->child) {
            child = p->child;
            p->child = NULL;
            chead = flatten_helper(child);
            ctail = chead->prev;
            chead->prev = p;
            ctail->next = p->next;
            if(p->next) {
                p->next->prev = ctail;
            }
            p->next = chead;
        }
        if(!ctail) {
            cout << "ctail is null" << endl;
        }
        while(ctail->next) {
            ctail = ctail->next;
        }
        head->prev = ctail;
        return head;
    }
    Node* flatten(Node* head) 
    {
        if(!head) return head;
        Node *ret = flatten_helper(head);
        ret->prev = NULL;
        return ret;
    }
};
// @lc code=end


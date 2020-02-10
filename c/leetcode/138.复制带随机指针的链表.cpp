/*
 * @lc app=leetcode.cn id=138 lang=cpp
 *
 * [138] 复制带随机指针的链表
 */
#include <iostream>
using namespace std;
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
// @lc code=start
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
*/
class Solution {
    void _print(Node *head) 
    {
        return;
        Node *p = head;
        if(p) {
            cout << "(" << p->val << ", " << (p->random ? p->random->val : -1) << ")";
            p = p->next;
        }
        while(p) {
            cout << " --> (" << p->val << ", " << (p->random ? p->random->val : -1) << ")";
            p = p->next;
        }
        cout << endl;
    }
    void _copy(Node *head)
    {
        Node *p = head;
        while(p) {
            Node *t = new Node(p->val);
            t->next = p->next;
            t->random = p->random;
            p->next = t;
            p = t->next;
        }
        p = head;
        while(p) {
            p = p->next;
            if(p->random) {
                p->random = p->random->next;
            }
            p = p->next;
        }
    }
public:
    Node* copyRandomList(Node* head) 
    {
        if(!head) return head;
        _print(head);
        _copy(head);
        _print(head);
        Node *p = head;
        Node *ret, *cur, *pre;
        ret = p->next;
        pre = NULL;
        while(p) {
            cur = p->next;
            if(pre) {
                pre->next = cur;
            }
            pre = cur;
            p->next = cur->next;
            p = p->next;
        }
        cur->next = NULL;
        _print(ret);
        _print(head);
        return ret;
    }
};
// @lc code=end


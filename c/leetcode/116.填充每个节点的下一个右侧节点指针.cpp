/*
 * @lc app=leetcode.cn id=116 lang=cpp
 *
 * [116] 填充每个节点的下一个右侧节点指针
 *
 * https://leetcode-cn.com/problems/populating-next-right-pointers-in-each-node/description/
 *
 * algorithms
 * Medium (51.16%)
 * Likes:    119
 * Dislikes: 0
 * Total Accepted:    20.1K
 * Total Submissions: 38.4K
 * Testcase Example:  '[1,2,3,4,5,6,7]'
 *
 * 给定一个完美二叉树，其所有叶子节点都在同一层，每个父节点都有两个子节点。二叉树定义如下：
 * 
 * struct Node {
 * ⁠ int val;
 * ⁠ Node *left;
 * ⁠ Node *right;
 * ⁠ Node *next;
 * }
 * 
 * 填充它的每个 next 指针，让这个指针指向其下一个右侧节点。如果找不到下一个右侧节点，则将 next 指针设置为 NULL。
 * 
 * 初始状态下，所有 next 指针都被设置为 NULL。
 * 
 * 
 * 
 * 示例：
 * 
 * 
 * 
 * 
 * 输入：{"$id":"1","left":{"$id":"2","left":{"$id":"3","left":null,"next":null,"right":null,"val":4},"next":null,"right":{"$id":"4","left":null,"next":null,"right":null,"val":5},"val":2},"next":null,"right":{"$id":"5","left":{"$id":"6","left":null,"next":null,"right":null,"val":6},"next":null,"right":{"$id":"7","left":null,"next":null,"right":null,"val":7},"val":3},"val":1}
 * 
 * 
 * 输出：{"$id":"1","left":{"$id":"2","left":{"$id":"3","left":null,"next":{"$id":"4","left":null,"next":{"$id":"5","left":null,"next":{"$id":"6","left":null,"next":null,"right":null,"val":7},"right":null,"val":6},"right":null,"val":5},"right":null,"val":4},"next":{"$id":"7","left":{"$ref":"5"},"next":null,"right":{"$ref":"6"},"val":3},"right":{"$ref":"4"},"val":2},"next":null,"right":{"$ref":"7"},"val":1}
 * 
 * 解释：给定二叉树如图 A 所示，你的函数应该填充它的每个 next 指针，以指向其下一个右侧节点，如图 B 所示。
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 你只能使用常量级额外空间。
 * 使用递归解题也符合要求，本题中递归程序占用的栈空间不算做额外的空间复杂度。
 * 
 * 
 */

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Node {
public:
    int val;
    Node* left;
    Node* right;
    Node* next;

    Node() : val(0), left(NULL), right(NULL), next(NULL) {}

    Node(int _val) : val(_val), left(NULL), right(NULL), next(NULL) {}

    Node(int _val, Node* _left, Node* _right, Node* _next)
        : val(_val), left(_left), right(_right), next(_next) {}
};

// @lc code=start
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* left;
    Node* right;
    Node* next;

    Node() : val(0), left(NULL), right(NULL), next(NULL) {}

    Node(int _val) : val(_val), left(NULL), right(NULL), next(NULL) {}

    Node(int _val, Node* _left, Node* _right, Node* _next)
        : val(_val), left(_left), right(_right), next(_next) {}
};
*/
class Solution {
public:
    struct Node_floor {
        Node *node;
        int floor;
        Node_floor() :node(NULL), floor(0) {}
        Node_floor(Node *n) :node(n), floor(0) {}
        Node_floor(Node *n, int f) :node(n), floor(f) {}
    };

    Node* connect(Node* root) 
    {
        #if 1
        if(!root) return NULL;
        // vector<Node_floor> q;
        queue<Node_floor> q;
        int f = 0;
        q.push(Node_floor(root, f));
        while(!q.empty()) {
            Node_floor tmp = q.front();
            if(tmp.node) {
                if(tmp.node->left) {
                    tmp.node->left->next = tmp.node->right;
                    q.push( Node_floor(tmp.node->left, tmp.floor + 1) );
                }
                if(tmp.node->right) {
                    q.push( Node_floor(tmp.node->right, tmp.floor + 1) );
                }
            }
            q.pop();
            if(!q.empty()) {
                Node_floor n = q.front();
                if(tmp.floor == n.floor) {
                    tmp.node->next = n.node;
                } else {
                    tmp.node->next = NULL;
                }
            } else {
                tmp.node->next = NULL;
            }
        }
        #else
        if(!root) return NULL;
        
        Node *left = root->left;
        Node *right = root->right;

        while(left) {
            left->next = right;
            left = left->right;
            right = right->left;
        }
        // if(root->left)
            connect(root->left);
        // if(root->right)
            connect(root->right);
        #endif
        return root;
    }
};
// @lc code=end


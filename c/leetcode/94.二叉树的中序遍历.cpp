/*
 * @lc app=leetcode.cn id=94 lang=cpp
 *
 * [94] 二叉树的中序遍历
 *
 * https://leetcode-cn.com/problems/binary-tree-inorder-traversal/description/
 *
 * algorithms
 * Medium (69.29%)
 * Likes:    357
 * Dislikes: 0
 * Total Accepted:    89.3K
 * Total Submissions: 128.8K
 * Testcase Example:  '[1,null,2,3]'
 *
 * 给定一个二叉树，返回它的中序 遍历。
 * 
 * 示例:
 * 
 * 输入: [1,null,2,3]
 * ⁠  1
 * ⁠   \
 * ⁠    2
 * ⁠   /
 * ⁠  3
 * 
 * 输出: [1,3,2]
 * 
 * 进阶: 递归算法很简单，你可以通过迭代算法完成吗？
 * 
 */
#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

// @lc code=start
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    void helper(TreeNode *root, vector<int> &ret) 
    {
        if(root) {
            helper(root->left, ret);
            ret.push_back(root->val);
            helper(root->right, ret);
        }
        
    }

    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> ret;
        #if 1
        vector<TreeNode*> stack;
        TreeNode *p = root;
        while(p || stack.size()) {
            while(p) {
                stack.push_back(p);
                p = p->left;
            }
            p = stack.back();
            ret.push_back(p->val);
            stack.pop_back();
            p = p->right;
        }
        #else
        helper(root, ret);
        #endif
        return ret;
    }
};
// @lc code=end


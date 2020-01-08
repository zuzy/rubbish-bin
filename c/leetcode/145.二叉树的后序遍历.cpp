/*
 * @lc app=leetcode.cn id=145 lang=cpp
 *
 * [145] 二叉树的后序遍历
 */
#include <iostream>
#include <vector>
#include <stack>

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
    vector<int> postorderTraversal(TreeNode* root) 
    {
        vector<int> ret;
        #if 0
        helper(root, ret);
        #else
        stack<TreeNode*> stk;
        if(root) {
            stk.push(root);
            while(!stk.empty()) {
                TreeNode* node = stk.top();
                stk.pop();
                ret.insert(ret.begin(), node->val);
                if(node->left) {
                    stk.push(node->left);
                }
                if(node->right) {
                    stk.push(node->right);
                }
            }
        }
        #endif
        return ret;
    }

    void helper(TreeNode *root, vector<int> &ret)
    {
        if(root) {
            helper(root->left, ret);
            helper(root->right, ret);
            ret.push_back(root->val);
        }
    }
};
// @lc code=end


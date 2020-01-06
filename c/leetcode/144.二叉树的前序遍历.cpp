/*
 * @lc app=leetcode.cn id=144 lang=cpp
 *
 * [144] 二叉树的前序遍历
 */

#include <iostream>
#include <vector>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

using namespace std;

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
    vector<int> preorderTraversal(TreeNode* root) {
        #if 1
        vector<int> ret;
        vector<TreeNode*> t_list;
        while(root || t_list.size()) {
            while(root) {
                // cout << "pushback " << root->val << endl; 
                ret.push_back(root->val);
                t_list.push_back(root);
                root = root->left;
            }
            root = t_list.back();
            root = root->right;
            t_list.pop_back();

        }
        return ret;

        #else
        vector<int> ret;
        helper(root, ret);
        return ret;
        #endif
    }

    void helper(TreeNode* root, vector<int> &ret) 
    {
        if(root) {
            ret.push_back(root->val);
            helper(root->left, ret);
            helper(root->right, ret);
        }
    }
};
// @lc code=end


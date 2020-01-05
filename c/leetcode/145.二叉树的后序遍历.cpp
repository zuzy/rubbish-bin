/*
 * @lc app=leetcode.cn id=145 lang=cpp
 *
 * [145] 二叉树的后序遍历
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
    vector<int> postorderTraversal(TreeNode* root) 
    {
        vector<int> ret;
        vector<TreeNode*> t_list;
        while(root || t_list.size()) {
            while(root) {
                t_list.push_back(root);
                root = root->left;
            }
            root = t_list.back();
            root = root->right;
            if(root->right)
            ret.push_back(root->val);
            t_list.pop_back();
        }
        return ret;
    }
};
// @lc code=end


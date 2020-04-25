/*
 * @lc app=leetcode.cn id=112 lang=cpp
 *
 * [112] 路径总和
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

    bool hasPathSum(TreeNode* root, int sum) {
        #if 0
        if(root == NULL) return false;
        if(root->left == NULL && root->right == NULL && root->val == sum) return true;
        return hasPathSum(root->right, sum - root->val) || hasPathSum(root->left, sum - root->val);
        #else 
        vector<TreeNode*> node_list;
        while(root || !node_list.empty()) {
            while(root) {
                node_list.push_back(root);
                root = root->left;
                sum -= root->val;
            }
            root = node_list.back();
            root = root->right;
            node_list.pop_back();
        }

        #endif
    }
};
// @lc code=end


/*
 * @lc app=leetcode.cn id=104 lang=cpp
 *
 * [104] 二叉树的最大深度
 *
 * https://leetcode-cn.com/problems/maximum-depth-of-binary-tree/description/
 *
 * algorithms
 * Easy (71.53%)
 * Likes:    423
 * Dislikes: 0
 * Total Accepted:    110.5K
 * Total Submissions: 154K
 * Testcase Example:  '[3,9,20,null,null,15,7]'
 *
 * 给定一个二叉树，找出其最大深度。
 * 
 * 二叉树的深度为根节点到最远叶子节点的最长路径上的节点数。
 * 
 * 说明: 叶子节点是指没有子节点的节点。
 * 
 * 示例：
 * 给定二叉树 [3,9,20,null,null,15,7]，
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
 * 
 * 返回它的最大深度 3 。
 * 
 */

#include <iostream>

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

    #if 0
    int deep_helper(TreeNode *root, int deep)
    {
        if(!root) return deep;

        int l_dp = deep_helper(root->left, deep);
        int r_dp = deep_helper(root->right, deep);
        
        return (l_dp > r_dp ? l_dp : r_dp) + 1;
    }
    #else
    int deep_helper(TreeNode *root, int deep)
    {
        if(!root) return deep;
        int l_dp , r_dp;
        l_dp = r_dp = deep + 1;
        if(root->left) {
            l_dp = deep_helper(root->left, l_dp);
        } 
        if(root->right) {
            r_dp = deep_helper(root->right, r_dp);
        }
        return l_dp > r_dp ? l_dp : r_dp;
    }
    #endif

    int maxDepth(TreeNode* root) 
    {
        return deep_helper(root, 0);
    }
};
// @lc code=end


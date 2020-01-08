/*
 * @lc app=leetcode.cn id=107 lang=cpp
 *
 * [107] 二叉树的层次遍历 II
 *
 * https://leetcode-cn.com/problems/binary-tree-level-order-traversal-ii/description/
 *
 * algorithms
 * Easy (63.48%)
 * Likes:    171
 * Dislikes: 0
 * Total Accepted:    36.2K
 * Total Submissions: 57K
 * Testcase Example:  '[3,9,20,null,null,15,7]'
 *
 * 给定一个二叉树，返回其节点值自底向上的层次遍历。 （即按从叶子节点所在层到根节点所在的层，逐层从左向右遍历）
 * 
 * 例如：
 * 给定二叉树 [3,9,20,null,null,15,7],
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
 * 
 * 
 * 返回其自底向上的层次遍历为：
 * 
 * [
 * ⁠ [15,7],
 * ⁠ [9,20],
 * ⁠ [3]
 * ]
 * 
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
    void _helper(vector<vector<int>> &out, TreeNode *root, int floor) 
    {
        if(out.size() <= floor) {
            #if 1
            out.push_back(vector<int> {});
            #else
            vector<int> tmp;
            out.emplace_back(tmp);
            #endif
        }
        out[floor].push_back(root->val);
        if(root->left)
            _helper(out, root->left, floor + 1);
        if(root->right)
            _helper(out, root->right, floor + 1);
    }

    vector<vector<int>> levelOrderBottom(TreeNode* root) {
        vector<vector<int>> ret;
        if(root) 
            _helper(ret, root, 0);
        
        return vector<vector<int>>(ret.rbegin(),ret.rend());
    }
};
// @lc code=end


/*
 * @lc app=leetcode.cn id=102 lang=cpp
 *
 * [102] 二叉树的层次遍历
 *
 * https://leetcode-cn.com/problems/binary-tree-level-order-traversal/description/
 *
 * algorithms
 * Medium (60.00%)
 * Likes:    345
 * Dislikes: 0
 * Total Accepted:    68.1K
 * Total Submissions: 113.1K
 * Testcase Example:  '[3,9,20,null,null,15,7]'
 *
 * 给定一个二叉树，返回其按层次遍历的节点值。 （即逐层地，从左到右访问所有节点）。
 * 
 * 例如:
 * 给定二叉树: [3,9,20,null,null,15,7],
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
 * 
 * 
 * 返回其层次遍历结果：
 * 
 * [
 * ⁠ [3],
 * ⁠ [9,20],
 * ⁠ [15,7]
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
    void helper(TreeNode *root, int floor, vector<vector<int>> &ret) {
        
        if(ret.size() <= floor) {
            vector<int> tmp;
            ret.emplace_back(tmp);
            cout << ret.size() << endl;
        }

        ret[floor].push_back(root->val);
        if(root->left)
            helper(root->left, floor+1, ret);
        if(root->right)
            helper(root->right, floor+1, ret);
        
    }

    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> ret;
        if(root)
            helper(root, 0, ret);
        return ret;
    }
};
// @lc code=end


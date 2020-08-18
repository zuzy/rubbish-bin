/*
 * @lc app=leetcode.cn id=110 lang=cpp
 *
 * [110] 平衡二叉树
 *
 * https://leetcode-cn.com/problems/balanced-binary-tree/description/
 *
 * algorithms
 * Easy (52.82%)
 * Likes:    408
 * Dislikes: 0
 * Total Accepted:    102.6K
 * Total Submissions: 193.2K
 * Testcase Example:  '[3,9,20,null,null,15,7]'
 *
 * 给定一个二叉树，判断它是否是高度平衡的二叉树。
 * 
 * 本题中，一棵高度平衡二叉树定义为：
 * 
 * 
 * 一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过1。
 * 
 * 
 * 示例 1:
 * 
 * 给定二叉树 [3,9,20,null,null,15,7]
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
 * 
 * 返回 true 。
 * 
 * 示例 2:
 * 
 * 给定二叉树 [1,2,2,3,3,null,null,4,4]
 * 
 * ⁠      1
 * ⁠     / \
 * ⁠    2   2
 * ⁠   / \
 * ⁠  3   3
 * ⁠ / \
 * ⁠4   4
 * 
 * 
 * 返回 false 。
 * 
 * 
 * 
 */

#include <iostream>
#include <vector>
#include <algorithm>
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
#if 0
    int high(TreeNode *root)
    {
        if(!root) return 0;
        return max(high(root->left), high(root->right)) + 1;
    }

    bool isBalanced(TreeNode* root) 
    {
        if(!root) return true;
        return (abs(high(root->left) - high(root->right)) <= 1) && isBalanced(root->left) && isBalanced(root->right);
    }
#else
    int hight(TreeNode *root)
    {
        if(!root) return 0;
        int hleft = hight(root->left);
        int hright = hight(root->right);
        if(hleft == -1 || hright == -1 || abs(hleft - hright) > 1) {
            return -1;
        }
        return max(hleft, hright) + 1;
    }

    bool isBalanced(TreeNode* root) 
    {
        return hight(root) != -1;
    }
#endif
};
// @lc code=end


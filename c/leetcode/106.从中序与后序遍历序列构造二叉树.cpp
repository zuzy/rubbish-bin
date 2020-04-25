/*
 * @lc app=leetcode.cn id=106 lang=cpp
 *
 * [106] 从中序与后序遍历序列构造二叉树
 *
 * https://leetcode-cn.com/problems/construct-binary-tree-from-inorder-and-postorder-traversal/description/
 *
 * algorithms
 * Medium (65.79%)
 * Likes:    139
 * Dislikes: 0
 * Total Accepted:    21.2K
 * Total Submissions: 32K
 * Testcase Example:  '[9,3,15,20,7]\n[9,15,7,20,3]'
 *
 * 根据一棵树的中序遍历与后序遍历构造二叉树。
 * 
 * 注意:
 * 你可以假设树中没有重复的元素。
 * 
 * 例如，给出
 * 
 * 中序遍历 inorder = [9,3,15,20,7]
 * 后序遍历 postorder = [9,15,7,20,3]
 * 
 * 返回如下的二叉树：
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
 * 
 * 
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

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
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        if(inorder.empty() || postorder.empty()) return NULL;
        TreeNode* root = new TreeNode(postorder.back());
        // cout << postorder.back() << endl;

        int i = find(inorder.begin(), inorder.end(), postorder.back()) - inorder.begin();
        // cout << "find " << i << "-> " << *(inorder.begin() + i) << endl;
        vector<int> l_in = vector<int>(inorder.begin(), inorder.begin() + i);
        vector<int> r_in = vector<int>(inorder.begin() + i + 1, inorder.end());
        
        postorder.pop_back();
        root->right = buildTree(r_in, postorder);
        root->left = buildTree(l_in, postorder);
        return root;
    }
};
// @lc code=end


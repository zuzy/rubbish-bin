/*
 * @lc app=leetcode.cn id=105 lang=cpp
 *
 * [105] 从前序与中序遍历序列构造二叉树
 *
 * https://leetcode-cn.com/problems/construct-binary-tree-from-preorder-and-inorder-traversal/description/
 *
 * algorithms
 * Medium (63.18%)
 * Likes:    306
 * Dislikes: 0
 * Total Accepted:    37.9K
 * Total Submissions: 59.8K
 * Testcase Example:  '[3,9,20,15,7]\n[9,3,15,20,7]'
 *
 * 根据一棵树的前序遍历与中序遍历构造二叉树。
 * 
 * 注意:
 * 你可以假设树中没有重复的元素。
 * 
 * 例如，给出
 * 
 * 前序遍历 preorder = [3,9,20,15,7]
 * 中序遍历 inorder = [9,3,15,20,7]
 * 
 * 返回如下的二叉树：
 * 
 * ⁠   3
 * ⁠  / \
 * ⁠ 9  20
 * ⁠   /  \
 * ⁠  15   7
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
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) 
    {
        if(preorder.empty() || inorder.empty()) return NULL;
        TreeNode* root = new TreeNode(preorder[0]);
        #if 1
        int i = find(inorder.begin(), inorder.end(), preorder[0]) - inorder.begin();
        // cout << *(inorder.begin() + i) << endl;
        vector<int> l_in = vector<int>(inorder.begin(), inorder.begin() + i);
        vector<int> r_in = vector<int>(inorder.begin() + i + 1, inorder.end());
        preorder.erase(preorder.begin());
        root->left = l_in.empty() ? NULL : buildTree(preorder, l_in);
        root->right = r_in.empty() ? NULL : buildTree(preorder, r_in);
        #else
        vector<int> l_in, r_in;

        auto i = inorder.begin();
        while(i != inorder.end() && *i != preorder[0]) {
            l_in.push_back(*i);
            ++i;
        }
        ++i;
        while(i != inorder.end()) {
            r_in.push_back(*i);
            ++i;
        }
        preorder.erase(preorder.begin());

        root->left = l_in.size() > 0 ? buildTree(preorder, l_in) : NULL;
        root->right = r_in.size() > 0 ? buildTree(preorder, r_in) : NULL;
        #endif

        return root;
    }
    
};
// @lc code=end


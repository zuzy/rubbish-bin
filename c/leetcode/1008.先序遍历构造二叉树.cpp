/*
 * @lc app=leetcode.cn id=1008 lang=cpp
 *
 * [1008] 先序遍历构造二叉树
 *
 * https://leetcode-cn.com/problems/construct-binary-search-tree-from-preorder-traversal/description/
 *
 * algorithms
 * Medium (71.93%)
 * Likes:    87
 * Dislikes: 0
 * Total Accepted:    7.8K
 * Total Submissions: 10.7K
 * Testcase Example:  '[8,5,1,7,10,12]'
 *
 * 返回与给定先序遍历 preorder 相匹配的二叉搜索树（binary search tree）的根结点。
 * 
 * (回想一下，二叉搜索树是二叉树的一种，其每个节点都满足以下规则，对于 node.left 的任何后代，值总 < node.val，而
 * node.right 的任何后代，值总 > node.val。此外，先序遍历首先显示节点的值，然后遍历 node.left，接着遍历
 * node.right。）
 * 
 * 
 * 
 * 示例：
 * 
 * 输入：[8,5,1,7,10,12]
 * 输出：[8,5,10,1,7,null,12]
 * 
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 1 <= preorder.length <= 100
 * 先序 preorder 中的值是不同的。
 * 
 * 
 */
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};
// @lc code=start
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
#if 1
    TreeNode *_helper(const vector<int> &preorder, int start, int end)
    {
        if(start >= end) return nullptr;
        TreeNode *node = new TreeNode(preorder[start]);
        ++start;
        for(int i = start; i < end; ++i) {
            // cout << i << " : " << preorder[i] << endl;
            if(preorder[i] > node->val) {
                // cout << "Find " << i << " : " << preorder[i] << endl;
                node->left = _helper(preorder, start, i);
                node->right = _helper(preorder, i, end);
                return node;
            }
        }
        node->left = _helper(preorder, start, end);
        return node;
    }

    TreeNode* bstFromPreorder(vector<int>& preorder)
    {
        return _helper(preorder, 0, preorder.size());
    }
#else
    TreeNode* bstFromPreorder(vector<int>& preorder)
    {
        if(preorder.size() == 0) {
            return nullptr;
        }
        int head = preorder[0];
        TreeNode *init = new TreeNode(head);

        for (int i = 1; i < preorder.size(); ++i) {
            // cout << preorder[i] << endl;
            if(preorder[i] > head) {
                // cout << "find " << i << " " << preorder[i] << endl;
                vector<int> l_in = vector<int>(preorder.begin() + 1, preorder.begin() + i);
                init->left = bstFromPreorder(l_in);
                vector<int> r_in = vector<int>(preorder.begin() + i, preorder.end());
                init->right = bstFromPreorder(r_in);
                return init;
            }
        }
        vector<int> l_in = vector<int>(preorder.begin() + 1, preorder.end());
        init->left = bstFromPreorder(l_in);

        return init;
    }
#endif
};
// @lc code=end


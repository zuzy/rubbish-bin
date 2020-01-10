/*
 * @lc app=leetcode.cn id=101 lang=cpp
 *
 * [101] 对称二叉树
 *
 * https://leetcode-cn.com/problems/symmetric-tree/description/
 *
 * algorithms
 * Easy (49.24%)
 * Likes:    563
 * Dislikes: 0
 * Total Accepted:    82.7K
 * Total Submissions: 167.2K
 * Testcase Example:  '[1,2,2,3,4,4,3]'
 *
 * 给定一个二叉树，检查它是否是镜像对称的。
 * 
 * 例如，二叉树 [1,2,2,3,4,4,3] 是对称的。
 * 
 * ⁠   1
 * ⁠  / \
 * ⁠ 2   2
 * ⁠/ \ / \
 * 3  4 4  3
 * 
 * 
 * 但是下面这个 [1,2,2,null,3,null,3] 则不是镜像对称的:
 * 
 * ⁠   1
 * ⁠  / \
 * ⁠ 2   2
 * ⁠  \   \
 * ⁠  3    3
 * 
 * 
 * 说明:
 * 
 * 如果你可以运用递归和迭代两种方法解决这个问题，会很加分。
 * 
 */
#include <iostream>
#include <vector>
#include <stack>

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
    bool is_mirror(TreeNode *left, TreeNode *right)
    {
        if(left == NULL && right == NULL) {
            return true;
        }
        if(left == NULL || right == NULL) {
            return false;
        }
        return (left->val == right->val) && is_mirror(left->left, right->right) && is_mirror(left->right, right->left);
    }

    bool isSymmetric(TreeNode* root) {
        #if 1
        stack<TreeNode*> stk_l, stk_r;
        if(!root) return true;
        TreeNode *l = root->left;
        TreeNode *r = root->right;
        while(l || r || (!stk_l.empty() && !stk_r.empty())) {
            if((l != NULL) ^ (r != NULL)) {
                return false;
            }
            while(l || r) {
                if(!l || !r) {
                    return false;
                }
                if(l->val != r->val) {
                    cout << "(" << l->val << "," << r->val << ")" << endl;
                    return false;
                }
                stk_l.push(l);
                stk_r.push(r);
                l = l->left;
                r = r->right;
            }
            l = stk_l.top();
            l = l->right;
            stk_l.pop();

            r = stk_r.top();
            r = r->left;
            stk_r.pop();
        }
        cout << "l size: " << stk_l.size() <<endl;
        cout << "r size: " << stk_r.size() <<endl;
        if(stk_l.size() || stk_r.size()) {
            return false;
        }
        return true;


        #else 
        return is_mirror(root, root);
        #endif 
    }
};
// @lc code=end


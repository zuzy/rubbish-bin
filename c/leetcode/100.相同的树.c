/*
 * @lc app=leetcode.cn id=100 lang=c
 *
 * [100] 相同的树
 */

#include <stdio.h>
#include <stdbool.h>
struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

// @lc code=start
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */


bool isSameTree(struct TreeNode* p, struct TreeNode* q)
{
    bool ret = true;
    if(p == NULL && q == NULL) return true;
    if((p && q) && (p->val == q->val)) {
        return ret & isSameTree(p->left, q->left) & isSameTree(p->right, q->right);
    }
    return false;
}


// @lc code=end


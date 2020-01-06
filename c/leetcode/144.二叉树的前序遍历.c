/*
 * @lc app=leetcode.cn id=144 lang=c
 *
 * [144] 二叉树的前序遍历
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void helper(struct TreeNode *root, int *returnSize, int **ret)
{
    if(root == NULL) {
        return;
    } else {
        ++*returnSize;
        *ret = (int *)realloc(*ret, *returnSize * sizeof(int));
        (*ret)[*returnSize - 1] = root->val;
        helper(root->left, returnSize, ret);
        helper(root->right, returnSize, ret);
    }
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* preorderTraversal(struct TreeNode* root, int* returnSize){
    int *ret = NULL;
    *returnSize = 0;
    helper(root, returnSize, &ret);
    return ret;
}


// @lc code=end


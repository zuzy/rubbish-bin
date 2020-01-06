/*
 * @lc app=leetcode.cn id=94 lang=c
 *
 * [94] 二叉树的中序遍历
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

static void helper(struct TreeNode *root, int **ret, int *pos, int *size)
{
    if(root) {
        helper(root->left, ret, pos, size);
        if(*pos >= *size) {
            *size += 10;
            *ret = realloc(*ret, *size * sizeof(int));
        }
        *(*ret + *pos) = root->val;
        ++*pos;
        helper(root->right, ret, pos, size);
    }
}

int* inorderTraversal(struct TreeNode* root, int* returnSize)
{
    int size = 0;
    int *ret = NULL;
    *returnSize = 0;
    helper(root, &ret, returnSize, &size);
    return ret;
}


// @lc code=end

int main(int argc, char *argv[])
{
    return 0;
}
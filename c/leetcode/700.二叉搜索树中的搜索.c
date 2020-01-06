/*
 * @lc app=leetcode.cn id=700 lang=c
 *
 * [700] 二叉搜索树中的搜索
 */

#include <stdio.h>
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
#define CASE 3
struct TreeNode* searchBST(struct TreeNode* root, int val){
#if CASE == 3
    while(root && root->val != val)
        root = val < root->val ? root->left : root->right;
    return root;
#endif

#if CASE == 2
    struct TreeNode *ptr = root;
    while(ptr) {
        if(ptr->val == val) {
            break;
        } else {
            if(val < ptr->val) {
                ptr = ptr->left;
            }else {
                ptr = ptr->right;
            }
        }
    }
    return ptr;
#endif

#if CASE == 1
    if(!root) return NULL;
    if(root->val == val) {
        return root;
    }
    if(val < root->val) {
        return searchBST(root->left, val);
    } else {
        return searchBST(root->right, val);
    }
#endif
}

// @lc code=end


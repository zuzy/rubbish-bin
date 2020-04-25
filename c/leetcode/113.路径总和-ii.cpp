/*
 * @lc app=leetcode.cn id=113 lang=cpp
 *
 * [113] 路径总和 II
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
    vector<vector<int>> pathSum(TreeNode* root, int sum) 
    {
        vector<vector<int>> ret;
        vector<TreeNode*> node_list;
        int cal = 0;

        while(root || !node_list.empty()) {
            while(root) {
                cal += root->val;
                node_list.push_back(root);
                root = root->left;
            }
            root = node_list.back();
            root = root->right;

            if(!root && cal == sum) { 
                vector<int> tmp;
                cout << "build tmp" << endl;
                for(TreeNode* i : node_list) {
                    cout << "\tpush: " << i->val << endl;
                    tmp.push_back(i->val);
                }
                ret.push_back(tmp);
            } else {
                
            }
            node_list.pop_back();


            if(root->right) {
                root = root->right;
            } else {
                if(cal == sum) {
                    vector<int> tmp;
                    cout << "build tmp" << endl;
                    for(TreeNode* i : node_list) {
                        cout << "\tpush: " << i->val << endl;
                        tmp.push_back(i->val);
                    }
                    ret.push_back(tmp);
                } 
            }
            
        }

        return ret;
    }
};
// @lc code=end


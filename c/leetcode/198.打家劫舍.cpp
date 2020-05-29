/*
 * @lc app=leetcode.cn id=198 lang=cpp
 *
 * [198] 打家劫舍
 *
 * https://leetcode-cn.com/problems/house-robber/description/
 *
 * algorithms
 * Easy (44.51%)
 * Likes:    815
 * Dislikes: 0
 * Total Accepted:    120.5K
 * Total Submissions: 267K
 * Testcase Example:  '[1,2,3,1]'
 *
 * 
 * 你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，影响你偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 * 
 * 给定一个代表每个房屋存放金额的非负整数数组，计算你 不触动警报装置的情况下 ，一夜之内能够偷窃到的最高金额。
 * 
 * 示例 1:
 * 
 * 输入: [1,2,3,1]
 * 输出: 4
 * 解释: 偷窃 1 号房屋 (金额 = 1) ，然后偷窃 3 号房屋 (金额 = 3)。
 * 偷窃到的最高金额 = 1 + 3 = 4 。
 * 
 * 示例 2:
 * 
 * 输入: [2,7,9,3,1]
 * 输出: 12
 * 解释: 偷窃 1 号房屋 (金额 = 2), 偷窃 3 号房屋 (金额 = 9)，接着偷窃 5 号房屋 (金额 = 1)。
 * 偷窃到的最高金额 = 2 + 9 + 1 = 12 。
 * 
 * 
 */
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// @lc code=start
class Solution {
public:
#if 1    
    int rob(vector<int>& nums) {
        auto s = nums.size();
        if(s == 0) {
            return 0;
        } else if(s == 1) {
            return nums[0];
        };
        int k1(nums[0]), k2(0), k(nums[0]);
        for(auto i = nums.begin() + 1; i != nums.end(); ++i) {
            k = max(k1, k2 + *i);
            // cout << *i << ", " << k2 << ", " << k1 << ", " << k << endl;
            k2 = k1;
            k1 = k;
        }
        return k1;
    }
#else 
int rob(vector<int>& nums) {
        auto s = nums.size();
        if(s == 0) {
            return 0;
        } else if(s == 1) {
            return nums[0];
        } else if(s == 2) {
            return max(nums[0], nums[1]);
        }
        vector<int> v1(nums.begin(), nums.end() - 1);
        vector<int> v2(nums.begin(), nums.end() - 2);
        return max(rob(v1), rob(v2) + nums[s - 1]);
    }
#endif
};
// @lc code=end


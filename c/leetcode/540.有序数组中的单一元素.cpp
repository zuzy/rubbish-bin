/*
 * @lc app=leetcode.cn id=540 lang=cpp
 *
 * [540] 有序数组中的单一元素
 *
 * https://leetcode-cn.com/problems/single-element-in-a-sorted-array/description/
 *
 * algorithms
 * Medium (59.72%)
 * Likes:    128
 * Dislikes: 0
 * Total Accepted:    13.9K
 * Total Submissions: 23.2K
 * Testcase Example:  '[1,1,2,3,3,4,4,8,8]'
 *
 * 给定一个只包含整数的有序数组，每个元素都会出现两次，唯有一个数只会出现一次，找出这个数。
 * 
 * 示例 1:
 * 
 * 
 * 输入: [1,1,2,3,3,4,4,8,8]
 * 输出: 2
 * 
 * 
 * 示例 2:
 * 
 * 
 * 输入: [3,3,7,7,10,11,11]
 * 输出: 10
 * 
 * 
 * 注意: 您的方案应该在 O(log n)时间复杂度和 O(1)空间复杂度中运行。
 * 
 */
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// @lc code=start

class Solution {
public:
    int singleNonDuplicate(vector<int>& nums) {
        int ret;
        _find(nums, 0, nums.size(), ret);
        return ret;
    }

    void _find(vector<int> const &nums, int start, int end, int &ret)
    {
        // cout << "start: " << start << ", end: " << end << endl;
        // for(int i = start; i < end; ++i) {
        //     cout << "\t" << i << ":" << nums[i] << endl;
        // }
        int len = end - start;
        if(len == 1) {
            ret = nums[start];
            return;
        }
        if(len == 3) {
            if(nums[start] == nums[start + 1]) {
                ret = nums[start + 2];
                return;
            } else {
                ret = nums[start];
                return;
            }
        }
        len >>= 2;
        len <<= 1;
        if(nums[len + start] == nums[len + start + 1]) {
            _find(nums, len + start + 2, end, ret);
        } else if(nums[len + start] == nums[len + start - 1]) {
            _find(nums, start, len + start - 1, ret);
        } else {
            ret = nums[len + start];
            return;
        }
    }
};

// @lc code=end


/*
 * @lc app=leetcode.cn id=287 lang=cpp
 *
 * [287] 寻找重复数
 *
 * https://leetcode-cn.com/problems/find-the-duplicate-number/description/
 *
 * algorithms
 * Medium (64.01%)
 * Likes:    627
 * Dislikes: 0
 * Total Accepted:    64.6K
 * Total Submissions: 99.2K
 * Testcase Example:  '[1,3,4,2,2]'
 *
 * 给定一个包含 n + 1 个整数的数组 nums，其数字都在 1 到 n 之间（包括 1 和
 * n），可知至少存在一个重复的整数。假设只有一个重复的整数，找出这个重复的数。
 * 
 * 示例 1:
 * 
 * 输入: [1,3,4,2,2]
 * 输出: 2
 * 
 * 
 * 示例 2:
 * 
 * 输入: [3,1,3,4,2]
 * 输出: 3
 * 
 * 
 * 说明：
 * 
 * 
 * 不能更改原数组（假设数组是只读的）。
 * 只能使用额外的 O(1) 的空间。
 * 时间复杂度小于 O(n^2) 。
 * 数组中只有一个重复的数字，但它可能不止重复出现一次。
 * 
 * 
 */
#include <vector>
#include <iostream>

using namespace std;

// @lc code=start
class Solution {
public:
    int findDuplicate(vector<int>& nums) 
    {
    #if 1
        int slow(0);
        int fast = nums[slow];
        cout << fast << "," << slow << endl;
        return nums[fast];
    #else
        int n = nums.size();
        int s(1), e(n - 1), ret(0);
        while(s < e) {
            int mid = (s + e) >> 1;
            int count = 0;
            for(int i = 0; i < n; ++i) {
                count += nums[i] <= mid; 
            }
            if(count > mid) {
                e = mid;
                // cout << "count > mid";
                // cout << "(" << s << ", " << e << ")" << endl;
            } else {
                s = mid + 1;
                // cout << "count <= mid";
                // cout << "(" << s << ", " << e << ")" << endl;
            }
        }
        return s;
    #endif
    }
};
// @lc code=end


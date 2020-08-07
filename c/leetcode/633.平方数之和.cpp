/*
 * @lc app=leetcode.cn id=633 lang=cpp
 *
 * [633] 平方数之和
 *
 * https://leetcode-cn.com/problems/sum-of-square-numbers/description/
 *
 * algorithms
 * Easy (33.25%)
 * Likes:    130
 * Dislikes: 0
 * Total Accepted:    26.3K
 * Total Submissions: 78.2K
 * Testcase Example:  '5'
 *
 * 给定一个非负整数 c ，你要判断是否存在两个整数 a 和 b，使得 a^2 + b^2 = c。
 * 
 * 示例1:
 * 
 * 
 * 输入: 5
 * 输出: True
 * 解释: 1 * 1 + 2 * 2 = 5
 * 
 * 
 * 
 * 
 * 示例2:
 * 
 * 
 * 输入: 3
 * 输出: False
 * 
 * 
 */
#include <iostream>
#include <cmath>
#include <cstdlib>
// @lc code=start
class Solution {
public:
    bool judgeSquareSum(int c) {
        if(!c) return true;
        long long s = sqrt(c);
        for(long long i = 0; i <= s;) {
            long long square = i * i + s * s;
            if(square == c) {
                return true;
            } else if(square > c) {
                --s;
            } else {
                ++i;
            }
        }
        return false;
    }
};
// @lc code=end


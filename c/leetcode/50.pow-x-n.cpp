/*
 * @lc app=leetcode.cn id=50 lang=cpp
 *
 * [50] Pow(x, n)
 *
 * https://leetcode-cn.com/problems/powx-n/description/
 *
 * algorithms
 * Medium (33.50%)
 * Likes:    327
 * Dislikes: 0
 * Total Accepted:    75.2K
 * Total Submissions: 215.5K
 * Testcase Example:  '2.00000\n10'
 *
 * 实现 pow(x, n) ，即计算 x 的 n 次幂函数。
 * 
 * 示例 1:
 * 
 * 输入: 2.00000, 10
 * 输出: 1024.00000
 * 
 * 
 * 示例 2:
 * 
 * 输入: 2.10000, 3
 * 输出: 9.26100
 * 
 * 
 * 示例 3:
 * 
 * 输入: 2.00000, -2
 * 输出: 0.25000
 * 解释: 2^-2 = 1/2^2 = 1/4 = 0.25
 * 
 * 说明:
 * 
 * 
 * -100.0 < x < 100.0
 * n 是 32 位有符号整数，其数值范围是 [−2^31, 2^31 − 1] 。
 * 
 * 
 */

// @lc code=start
class Solution {
public:
    /**
     * @brief  
     * @note   
     * @param  x: 
     * @param  n: long long int, fucking test case (1.000 & -2147483648)
     * @retval 
     */
    double myPow(double x, long long int n) {
        if(x == 0) return 0;
        if(n == 0) return 1;
        if(n == 1) return x;
        if(n < 0) return 1.0/myPow(x, -n);
        double sq = myPow(x, n / 2);
        return sq * sq * myPow(x, n % 2);
    }
};
// @lc code=end


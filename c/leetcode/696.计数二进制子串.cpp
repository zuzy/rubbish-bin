/*
 * @lc app=leetcode.cn id=696 lang=cpp
 *
 * [696] 计数二进制子串
 *
 * https://leetcode-cn.com/problems/count-binary-substrings/description/
 *
 * algorithms
 * Easy (52.08%)
 * Likes:    180
 * Dislikes: 0
 * Total Accepted:    14.6K
 * Total Submissions: 26.1K
 * Testcase Example:  '"00110"'
 *
 * 给定一个字符串 s，计算具有相同数量0和1的非空(连续)子字符串的数量，并且这些子字符串中的所有0和所有1都是组合在一起的。
 * 
 * 重复出现的子串要计算它们出现的次数。
 * 
 * 示例 1 :
 * 
 * 
 * 输入: "00110011"
 * 输出: 6
 * 解释: 有6个子串具有相同数量的连续1和0：“0011”，“01”，“1100”，“10”，“0011” 和 “01”。
 * 
 * 请注意，一些重复出现的子串要计算它们出现的次数。
 * 
 * 另外，“00110011”不是有效的子串，因为所有的0（和1）没有组合在一起。
 * 
 * 
 * 示例 2 :
 * 
 * 
 * 输入: "10101"
 * 输出: 4
 * 解释: 有4个子串：“10”，“01”，“10”，“01”，它们具有相同数量的连续1和0。
 * 
 * 
 * 注意：
 * 
 * 
 * s.length 在1到50,000之间。
 * s 只包含“0”或“1”字符。
 * 
 * 
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// @lc code=start
class Solution {
public:
    int countBinarySubstrings(string s) 
    {
        int ret = 0;
        if(s.length() < 2) {
            return 0;
        }
        auto c = s.begin();
        char p = *c;
        int pre = 0;
        int cur = 1;
        
        for(++c; c != s.end(); ++c) {
            if(*c == p) {
                ++cur;
            }else {
                p = *c;
                ret += pre < cur ? pre : cur;
                pre = cur;
                cur = 1;
            }
        }
        ret += pre < cur ? pre : cur;
        return ret;
    }
};
// @lc code=end


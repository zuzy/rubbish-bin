/*
 * @lc app=leetcode.cn id=43 lang=cpp
 *
 * [43] 字符串相乘
 *
 * https://leetcode-cn.com/problems/multiply-strings/description/
 *
 * algorithms
 * Medium (42.61%)
 * Likes:    412
 * Dislikes: 0
 * Total Accepted:    80.4K
 * Total Submissions: 187.5K
 * Testcase Example:  '"2"\n"3"'
 *
 * 给定两个以字符串形式表示的非负整数 num1 和 num2，返回 num1 和 num2 的乘积，它们的乘积也表示为字符串形式。
 * 
 * 示例 1:
 * 
 * 输入: num1 = "2", num2 = "3"
 * 输出: "6"
 * 
 * 示例 2:
 * 
 * 输入: num1 = "123", num2 = "456"
 * 输出: "56088"
 * 
 * 说明：
 * 
 * 
 * num1 和 num2 的长度小于110。
 * num1 和 num2 只包含数字 0-9。
 * num1 和 num2 均不以零开头，除非是数字 0 本身。
 * 不能使用任何标准库的大数类型（比如 BigInteger）或直接将输入转换为整数来处理。
 * 
 * 
 */

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

// @lc code=start

class Solution {
public:
#if 1

    string multiply(string num1, string num2) 
    {
        if(num1 == "0" || num2 == "0") {
            return "0";
        }
        int base = 0;
        string ret = "0";
        for (int i = num1.size() - 1; i >= 0; --i, ++base) {
            add(ret, rtimes(num2, num1[i] - '0', base));
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }
    
    string rtimes(string num, int times, int base)
    {
        string ret(base, '0');
        int carry = 0;
        for (int i = num.size() - 1; i >= 0; --i) {
            int product = (num[i] - '0') * times + carry;
            carry = product / 10;
            ret += product % 10 + '0';
        }
        if(carry) {
            ret += carry + '0';
        }
        return ret;
    }

    void add(string &num, string const &addend)
    {
        int carry = 0;
        int i = 0;
        int j = 0;
        for(; i < num.size() || j < addend.size() || carry; ++i, ++j) {
            int sum = carry;
            if(j < addend.size()) {
                sum += addend[j] - '0';
            }
            if(i < num.size()) {
                sum += num[i] - '0';
                num[i] = sum % 10 + '0';
            } else {
                num += sum % 10 + '0';
            }
            carry = sum / 10;
        }
    }
#else
    string multiply(string num1, string num2) 
    {
        int base = 0;
        if(num1 == "0" || num2 == "0") {
            return "0";
        }
        string ret = "0";
        for (int i = num1.size() - 1; i >= 0; --i, ++base) {
            // ret = add(times(num2, num1[i] - '0', base), ret);
            add(ret, times(num2, num1[i] - '0', base));
            // cout << "ret:" << ret << endl;
        }
        return ret;
    }
    
    string times(string num, int times, int base)
    {
        string ret;
        while(base) {
            ret += '0';
            --base;
        }
        int carry = 0;
        for (int i = num.size() - 1; i >= 0; --i) {
            int product = (num[i] - '0') * times + carry;
            carry = product / 10;
            ret += product % 10 + '0';
            // cout << ret << endl;
        }
        if(carry) {
            ret += carry + '0';
        }
        reverse(ret.begin(), ret.end());
        // cout << num << " * " << times << " = " << ret << endl;
        return ret;
    }
    #if 0
    string add(string const &num1, string const &num2)
    {
        string ret;
        int i = num1.size() - 1;
        int j = num2.size() - 1;
        int carry = 0;
        for(; i >= 0 || j >= 0 || carry; --i, --j) {
            int sum = 0;
            if(i >= 0) {
                sum += num1[i] - '0';
            }
            if(j >= 0) {
                sum += num2[j] - '0';
            }
            sum += carry;
            ret.push_back(sum % 10 + '0');
            carry = sum / 10;
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }
    #else
    void add(string &num, string const &addend)
    {
        reverse(num.begin(), num.end());
        int carry = 0;
        int i = 0;
        int j = addend.size() - 1;
        // while(true) {
        for(; i < num.size() || j >= 0 || carry; ++i, --j) {
            int sum = carry;
            if(j >= 0) {
                sum += addend[j] - '0';
            }
            if(i < num.size()) {
                sum += num[i] - '0';
                num[i] = sum % 10 + '0';
            } else {
                num += sum % 10 + '0';
            }
            carry = sum / 10;
        }
        reverse(num.begin(), num.end());
    }
    #endif
#endif
};

// @lc code=end


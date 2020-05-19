/*
 * @lc app=leetcode.cn id=680 lang=cpp
 *
 * [680] 验证回文字符串 Ⅱ
 *
 * https://leetcode-cn.com/problems/valid-palindrome-ii/description/
 *
 * algorithms
 * Easy (36.65%)
 * Likes:    155
 * Dislikes: 0
 * Total Accepted:    21.8K
 * Total Submissions: 58.5K
 * Testcase Example:  '"aba"'
 *
 * 给定一个非空字符串 s，最多删除一个字符。判断是否能成为回文字符串。
 * 
 * 示例 1:
 * 
 * 
 * 输入: "aba"
 * 输出: True
 * 
 * 
 * 示例 2:
 * 
 * 
 * 输入: "abca"
 * 输出: True
 * 解释: 你可以删除c字符。
 * 
 * 
 * 注意:
 * 
 * 
 * 字符串只包含从 a-z 的小写字母。字符串的最大长度是50000。
 * 
 * 
 */
#include <iostream>
#include <string>
using namespace std;
string test_string = "aguokepatgbnvfqmgmlcupuufxoohdfpgjdmysgvhmvffcnqxjjxqncffvmhvgsymdjgpfdhooxfuupuculmgmqfvnbgtapekouga";

// "cupuufxoohdfpgjdmysgvhmvffcnqxjjxqncffvmhvgsymdjgpfdhooxfuupucu"
// @lc code=start
class Solution {
    bool del;
public:
    Solution():del(false){}

    bool validPalindrome(string s)
    {
#if 1
        // cout << "[input] " << s << " " << del << endl;
        for(int start(0), end(s.size() - 1); start < end; ++start, --end) {
            if(s[start] != s[end]) {
                if(del) return false;
                else {
                    del = true;
                    string ss = s.substr(start, end - start + 1);
                    // cout << ss << endl;
                    return validPalindrome(ss.substr(0, ss.size() - 1)) || validPalindrome(ss.substr(1, ss.size() - 1));
                }
            }
        }
        return true;
    }    
#else
        string sub = palindrome(s);
        if(sub.empty())
            return true;
        if(palindrome(sub.substr(1, sub.size() - 1)).empty() || palindrome(sub.substr(0, sub.size() - 1)).empty()) {
            return true;
        }
        return false;
    }

    string palindrome(string s)
    {
        for(int start(0), end(s.size() - 1); start < end; ++start, --end) {
            if(s[start] != s[end]) return s.substr(start, end - start + 1);
        }
        return string();
    }
#endif
};
// @lc code=end


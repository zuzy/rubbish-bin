/*
 * @lc app=leetcode.cn id=5 lang=cpp
 *
 * [5] 最长回文子串
 *
 * https://leetcode-cn.com/problems/longest-palindromic-substring/description/
 *
 * algorithms
 * Medium (29.64%)
 * Likes:    2180
 * Dislikes: 0
 * Total Accepted:    265.5K
 * Total Submissions: 882.2K
 * Testcase Example:  '"babad"'
 *
 * 给定一个字符串 s，找到 s 中最长的回文子串。你可以假设 s 的最大长度为 1000。
 * 
 * 示例 1：
 * 
 * 输入: "babad"
 * 输出: "bab"
 * 注意: "aba" 也是一个有效答案。
 * 
 * 
 * 示例 2：
 * 
 * 输入: "cbbd"
 * 输出: "bb"
 * 
 * 
 */
#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

// @lc code=start
#define SEL 2
class Solution {
public:
#if SEL == 2
    pair<int, int> expend(string &s, int left, int right)
    {
        int len = s.size();
        for(;left >= 0 && right < len && s[left] == s[right]; --left, ++right);
        // return pair<int, int>(left + 1, right - 1);
        return {left + 1, right - 1};
    }

    string longestPalindrome(string s) 
    {
        if(s.size() <= 1) {
            return s;
        }
        int n = s.size();
        string ret = s.substr(0, 1);
        for(int i = 0; i < n; ++i) {
            pair<int, int> ex1 = expend(s, i, i);
            pair<int, int> ex2 = expend(s, i, i + 1);
            pair<int, int> max;
            if((ex1.second - ex1.first) > (ex2.second - ex2.first)) {
                max = ex1;
            } else {
                max = ex2;
            }
            if(ret.size() < (max.second - max.first + 1)) {
                ret = s.substr(max.first, max.second - max.first + 1);
            }
        }
        return ret;
    }
#elif SEL== 1
    string longestPalindrome(string s) {
        int n = s.size();
        vector<vector<int>> dp(n, vector<int>(n));
        string ans;
        for (int l = 0; l < n; ++l) {
            for (int i = 0; i + l < n; ++i) {
                int j = i + l;
                if (l == 0) {
                    dp[i][j] = 1;
                }
                else if (l == 1) {
                    dp[i][j] = (s[i] == s[j]);
                }
                else {
                    dp[i][j] = (s[i] == s[j] && dp[i + 1][j - 1]);
                }
                if (dp[i][j] && l + 1 > ans.size()) {
                    ans = s.substr(i, l + 1);
                }
            }
        }
        return ans;
    }
#else
    bool _check(string s)
    {
        int st(0), end(s.size() - 1);
        while(st < end) {
            if(s[st] != s[end]) {
                return false;
            }
            ++st;
            --end;
        }
        return true;
    }

    string longestPalindrome(string s) 
    {
        int n = s.size();
        if(n <= 1) {
            return s;
        }
        string ret = s.substr(0, 1);
        for(int i = 0; ret.size() < n -i; ++i) {
            for(int j = n; j - i > ret.size(); --j) {
                string cut = s.substr(i, j - i);
                if(_check(cut)) {
                    if(ret.size() < cut.size()) {
                        ret = cut;
                        break;
                    }
                }
            }
        }
        return ret;
    }
#endif
};
// @lc code=end

// "aaabaaaa"
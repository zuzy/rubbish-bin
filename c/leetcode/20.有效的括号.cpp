/*
 * @lc app=leetcode.cn id=20 lang=cpp
 *
 * [20] 有效的括号
 *
 * https://leetcode-cn.com/problems/valid-parentheses/description/
 *
 * algorithms
 * Easy (42.14%)
 * Likes:    1768
 * Dislikes: 0
 * Total Accepted:    364.3K
 * Total Submissions: 858.1K
 * Testcase Example:  '"()"'
 *
 * 给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串，判断字符串是否有效。
 * 
 * 有效字符串需满足：
 * 
 * 
 * 左括号必须用相同类型的右括号闭合。
 * 左括号必须以正确的顺序闭合。
 * 
 * 
 * 注意空字符串可被认为是有效字符串。
 * 
 * 示例 1:
 * 
 * 输入: "()"
 * 输出: true
 * 
 * 
 * 示例 2:
 * 
 * 输入: "()[]{}"
 * 输出: true
 * 
 * 
 * 示例 3:
 * 
 * 输入: "(]"
 * 输出: false
 * 
 * 
 * 示例 4:
 * 
 * 输入: "([)]"
 * 输出: false
 * 
 * 
 * 示例 5:
 * 
 * 输入: "{[]}"
 * 输出: true
 * 
 */

#include <iostream>
#include <stack>
#include <string>
#include <map>

using namespace std;

// @lc code=start

class Solution {
public:
    bool isValid(string s) {
        const map<char, char> mp = {{')', '('}, {']', '['}, {'}', '{'}};
        if(s.length() % 2) return false;
        stack<char> brace;
        for(auto &c:s) {
            switch(c) {
                case '{':
                case '[':
                case '(': {
                    brace.push(c);
                    break;
                }
                case '}':
                case ']':
                case ')': {
                    // cout << mp.at(c) << endl;
                    if(brace.empty()) return false;
                    if(mp.at(c) == brace.top()) {
                        brace.pop();
                    } else {
                        return false;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return brace.empty();
    }
};

// @lc code=end


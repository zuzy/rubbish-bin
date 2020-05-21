/*
 * @lc app=leetcode.cn id=1371 lang=cpp
 *
 * [1371] 每个元音包含偶数次的最长子字符串
 *
 * https://leetcode-cn.com/problems/find-the-longest-substring-containing-vowels-in-even-counts/description/
 *
 * algorithms
 * Medium (43.48%)
 * Likes:    55
 * Dislikes: 0
 * Total Accepted:    2.3K
 * Total Submissions: 4.7K
 * Testcase Example:  '"eleetminicoworoep"'
 *
 * 给你一个字符串 s ，请你返回满足以下条件的最长子字符串的长度：每个元音字母，即 'a'，'e'，'i'，'o'，'u'
 * ，在子字符串中都恰好出现了偶数次。
 * 
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：s = "eleetminicoworoep"
 * 输出：13
 * 解释：最长子字符串是 "leetminicowor" ，它包含 e，i，o 各 2 个，以及 0 个 a，u 。
 * 
 * 
 * 示例 2：
 * 
 * 
 * 输入：s = "leetcodeisgreat"
 * 输出：5
 * 解释：最长子字符串是 "leetc" ，其中包含 2 个 e 。
 * 
 * 
 * 示例 3：
 * 
 * 
 * 输入：s = "bcbcbc"
 * 输出：6
 * 解释：这个示例中，字符串 "bcbcbc" 本身就是最长的，因为所有的元音 a，e，i，o，u 都出现了 0 次。
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 1 <= s.length <= 5 x 10^5
 * s 只包含小写英文字母。
 * 
 * 
 */
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;
// @lc code=start
class Solution {
public:

#if 0
    int findTheLongestSubstring(string s) 
    {
        map<char, int> vowel;
        vowel.clear();
        int ret = 0;
        for(int i = 0; i < s.size(); ++i) {
            switch (s[i]) {
            case 'a':
            case 'e':
            case 'i':
            case 'o':
            case 'u': {
                ++vowel[s[i]];
            }
            default:
                break;
            }
        }
        bool even = true;
        #if 1
        for(auto it = vowel.begin(); it != vowel.end(); ++it) {
            if(it->second % 2) {
                even = false;
                string::size_type n = s.find(it->first);
                string left = s.substr(n + 1);
                n = s.rfind(it->first);
                string right = s.substr(0, n);
                string max;
                if(left.size() > right.size()) {
                    max = left;
                } else {
                    max = right;
                }
                if(max.size() > ret) {
                    int r = findTheLongestSubstring(max);
                    ret = ret > r ? ret : r;
                }
            }
        }
        #else
        for(auto it = vowel.begin(); it != vowel.end(); ++it) {
            if(it->second % 2) {
                // cout << "(" << it->first << ": " << it->second << ")"<< endl;
                even = false;
                string::size_type n = s.find(it->first);
                string left = s.substr(n + 1);
                n = s.rfind(it->first);
                string right = s.substr(0, n);
                if(left.size() > right.size()) {
                    if(left.size() > ret) {
                        int l = findTheLongestSubstring(left);
                        ret = ret > l ? ret : l;
                        if(right.size() > ret) {
                            int r = findTheLongestSubstring(right);
                            ret = ret > r ? ret: r;
                        }
                    }
                } else {
                    if(right.size() > ret) {
                        int r = findTheLongestSubstring(right);
                        ret = ret > r ? ret : r;
                        if(left.size() > ret) {
                            int l = findTheLongestSubstring(left);
                            ret = ret > l ? ret: l;
                        }
                    }
                }
            }
        }
        #endif
        if(even) {
            ret = s.size();
        }

        return ret;
    }
#else
    int findTheLongestSubstring(string s) 
    {
        vector <int> pos(1 << 5, -1);
        int status(0), ret(0), n(s.size());
        map <char, int> pre;
        
        for(int i = 0; i < n; i++) {
            switch (s[i])
            {
            case 'a': {
                status ^= 1;
                break;
            }
            case 'e': {
                status ^= 1 << 1;
                break;
            }
            case 'i': {
                status ^= 1 << 2;
                break;
            }
            case 'o': {
                status ^= 1 << 3;
                break;
            }
            case 'u': {
                status ^= 1 << 4;
                break;
            }
            default:
                break;
            }
            #if 1
            if(status == 0) {
                ret = i + 1;
            } else {
                if(pos[status] == -1) {
                    pos[status] = i;
                } else {
                    int l = i - pos[status];
                    ret = ret > l ? ret : l;
                }
            }
            #else
            if (~pos[status]) {
                ret = max(ret, i + 1 - pos[status]);
            } else {
                pos[status] = i + 1;
            }
            #endif
            // cout << "[" << i << "]" << s.substr(0, i+1) << "[end]: " << hex << status << dec << ", " << pos[status] << " ret: " << ret << endl;
        }

        return ret;
    }
#endif
};
// @lc code=end
//"leetcodeisgreat"

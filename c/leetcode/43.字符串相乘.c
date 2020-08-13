/*
 * @lc app=leetcode.cn id=43 lang=c
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
#include <string.h>
// @lc code=start

#define MAX_LEN 220
#define FINAL MAX_LEN-1
char* multiply(char* num1, char* num2) {
    if(strcmp(num1, "0") == 0 || strcmp(num2, "0") == 0) {
        return strdup("0");
    }
    char *ret = calloc(1, MAX_LEN);
    char *pr = ret + FINAL;
    unsigned int *ret_int = calloc(MAX_LEN, sizeof(int));
    unsigned int *p;
    char *p1 = num1;
    char *p2;
    int i = 1;
    while(*p1) {
        p2 = num2;
        p = ret_int + i;
        while(*p2) {
            *p += (*p1 - '0') * (*p2 - '0');
            ++p;
            ++p2;
        }
        ++p1;
        ++i;
    }


    --p;
    while(p >= ret_int) {        
        if(*p > 100) {
            *(p-2) += *p / 100;
            *p = *p % 100;
        }
        if(*p >= 10) {
            *(p-1) += *p / 10;
            *p = *p % 10;
        }
        // *p += '0';
        *pr = *p + '0';
        --pr;
        --p;
    }
    if(*ret == '0' || *ret == 0) {
        char *r = malloc(220);
        int i = 0;
        while(i < MAX_LEN && (ret[i] == '0' || ret[i] == '\0')){
            ++i;
        }
        if(i == MAX_LEN) {
            r[0] = '0';
        } else {
            strcpy(r, ret+i);
        }
        free(ret);
        return r;
    } else {
        return ret;
    }
}

// @lc code=end


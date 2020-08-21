/*
 * @lc app=leetcode.cn id=529 lang=cpp
 *
 * [529] 扫雷游戏
 *
 * https://leetcode-cn.com/problems/minesweeper/description/
 *
 * algorithms
 * Medium (60.62%)
 * Likes:    106
 * Dislikes: 0
 * Total Accepted:    12.8K
 * Total Submissions: 20.4K
 * Testcase Example:  '[["E","E","E","E","E"],["E","E","M","E","E"],["E","E","E","E","E"],["E","E","E","E","E"]]\n' +
  '[3,0]'
 *
 * 让我们一起来玩扫雷游戏！
 * 
 * 给定一个代表游戏板的二维字符矩阵。 'M' 代表一个未挖出的地雷，'E' 代表一个未挖出的空方块，'B'
 * 代表没有相邻（上，下，左，右，和所有4个对角线）地雷的已挖出的空白方块，数字（'1' 到 '8'）表示有多少地雷与这块已挖出的方块相邻，'X'
 * 则表示一个已挖出的地雷。
 * 
 * 现在给出在所有未挖出的方块中（'M'或者'E'）的下一个点击位置（行和列索引），根据以下规则，返回相应位置被点击后对应的面板：
 * 
 * 
 * 如果一个地雷（'M'）被挖出，游戏就结束了- 把它改为 'X'。
 * 如果一个没有相邻地雷的空方块（'E'）被挖出，修改它为（'B'），并且所有和其相邻的未挖出方块都应该被递归地揭露。
 * 如果一个至少与一个地雷相邻的空方块（'E'）被挖出，修改它为数字（'1'到'8'），表示相邻地雷的数量。
 * 如果在此次点击中，若无更多方块可被揭露，则返回面板。
 * 
 * 
 * 
 * 
 * 示例 1：
 * 
 * 输入: 
 * 
 * [['E', 'E', 'E', 'E', 'E'],
 * ⁠['E', 'E', 'M', 'E', 'E'],
 * ⁠['E', 'E', 'E', 'E', 'E'],
 * ⁠['E', 'E', 'E', 'E', 'E']]
 * 
 * Click : [3,0]
 * 
 * 输出: 
 * 
 * [['B', '1', 'E', '1', 'B'],
 * ⁠['B', '1', 'M', '1', 'B'],
 * ⁠['B', '1', '1', '1', 'B'],
 * ⁠['B', 'B', 'B', 'B', 'B']]
 * 
 * 解释:
 * 
 * 
 * 
 * 示例 2：
 * 
 * 输入: 
 * 
 * [['B', '1', 'E', '1', 'B'],
 * ⁠['B', '1', 'M', '1', 'B'],
 * ⁠['B', '1', '1', '1', 'B'],
 * ⁠['B', 'B', 'B', 'B', 'B']]
 * 
 * Click : [1,2]
 * 
 * 输出: 
 * 
 * [['B', '1', 'E', '1', 'B'],
 * ⁠['B', '1', 'X', '1', 'B'],
 * ⁠['B', '1', '1', '1', 'B'],
 * ⁠['B', 'B', 'B', 'B', 'B']]
 * 
 * 解释:
 * 
 * 
 * 
 * 
 * 
 * 注意：
 * 
 * 
 * 输入矩阵的宽和高的范围为 [1,50]。
 * 点击的位置只能是未被挖出的方块 ('M' 或者 'E')，这也意味着面板至少包含一个可点击的方块。
 * 输入面板不会是游戏结束的状态（即有地雷已被挖出）。
 * 简单起见，未提及的规则在这个问题中可被忽略。例如，当游戏结束时你不需要挖出所有地雷，考虑所有你可能赢得游戏或标记方块的情况。
 * 
 * 
 */
#include <iostream>
#include <vector>
#include <string>

using namespace std;


// @lc code=start
class Solution {
public:
    using callback = int (*)(vector<vector<char>>&, int, int);
    // typedef int (*callback) (vector<vector<char>> &b, int x, int y);
    static int loop(vector<vector<char>> &board, int x, int y, callback fun) 
    {
        int ret = 0;
        if(x < 0 || y < 0 || x >= board.size() || y >= board[x].size()) {
            return ret;
        }
        for (int i(-1); i < 2; ++i) {
            for(int j(-1); j < 2; ++j) {
                int x0(x + i), y0(y + j);
                if(x0 >= 0 && y0 >= 0 && x0 < board.size() && y0 < board[x0].size()) {
                    if(board[x0][y0] == 'B' || board[x0][y0] == 'X') {
                        continue;
                    }
                    ret += fun(board, x0, y0);
                }
            }
        }
        return ret;
    }

    static int count(vector<vector<char>> &board, int x, int y)
    {
        return (board[x][y] == 'M' || board[x][y] == 'X') ? 1: 0;
    }
    
    static int convert(vector<vector<char>> &board, int x, int y)
    {
        int ret = 0;
        if(board[x][y] == 'E') {
            ret = loop(board, x, y, count);
            if(ret) {
                board[x][y] = ret + '0';
                
            } else {
                board[x][y] = 'B';
                loop(board, x, y, convert);
            }
        }
        return ret;
    }

    vector<vector<char>> updateBoard(vector<vector<char>>& board, vector<int>& click) 
    {
        int x(click[0]), y(click[1]);
        if(board[x][y] == 'M') {
            board[x][y] = 'X';
            return board;
        }
        if(convert(board, x, y) == 0) {
            loop(board, click[0], click[1], convert);
        }
        return board;
    }
};
// @lc code=end


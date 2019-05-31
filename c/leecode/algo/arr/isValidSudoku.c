#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define INT_SIZE	(sizeof(int))
#define S   9
#define print_common(format, arg...)   do { printf("%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
static bool _check(char **board, char des, int row, int col) {
    int i, j;
    for(i = 0; i < S; ++i) {
        if(i != col) {
            if(board[row][i] == des) {
                return false;
            }
        }
        if(i != row) {
            if(board[i][col] == des) {
                return false;
            }
        }
    }
    int _row, _col;
    _row = (row / 3) * 3;
    _col = (col / 3) * 3;
    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(_row + i != row || _col + j != col) {
                if(board[_row + i][_col + j] == des) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isValidSudoku(char** board, int boardRowSize, int boardColSize) {
    int i, j;
    for(i = 0; i < boardRowSize; ++i) {
        for(j = 0; j < boardColSize; ++j) {
            if(board[i][j] == '.') continue;
            if(_check(board, board[i][j], i, j) == false){
                return false;
            }
        }
    }
    return true;
}

#define NUM 10

char **a = 
[["5","3",".",".","7",".",".",".","."],["6",".",".","1","9","5",".",".","."],[".","9","8",".",".",".",".","6","."],["8",".",".",".","6",".",".",".","3"],["4",".",".","8",".","3",".",".","1"],["7",".",".",".","2",".",".",".","6"],[".","6",".",".",".",".","2","8","."],[".",".",".","4","1","9",".",".","5"],[".",".",".",".","8",".",".","7","9"]]


int main(int argc, char *argv[])
{

    return 0;
}
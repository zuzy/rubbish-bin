#include <stdlib.h>
#include <stdio.h>

static int count(int **board, int m_r, int m_c, int r, int c)
{
    int count = 0;
    if(0 <= r - 1 && 0 <= c - 1) {
        if(board[r-1][c-1]) ++count;
    }
    if(0 <= r - 1 && 0 <= c) {
        if(board[r-1][c]) ++count;
    }
    if(0 <= r - 1 && m_c >= c + 1) {
        if(board[r-1][c+1]) ++count;
    }
    if(0 <= c - 1) {
        if(board[r][c-1]) ++count;
    }
    if(board[r][c]) ++count;
    if(m_c >= c + 1) {
        if(board[r][c+1]) ++count;
    }
    if(m_r >= r + 1 && 0 <= c - 1) {
        if(board[r+1][c-1]) ++count;
    }
    if(m_r >= r + 1) {
        if(board[r+1][c]) ++count;
    }
    if(m_r >= r + 1 && m_c >= c + 1) {
        if(board[r+1][c+1]) ++count;
    }
    return count;
}

void gameOfLife(int** board, int boardRowSize, int *boardColSizes) {
    int **tmp = calloc(boardRowSize, sizeof(int*));
    int i, j;
    for(i = 0; i < boardRowSize; i++) {
        tmp[i] = malloc(sizeof(int) * (*boardColSizes));
        for(j = 0; j < *boardColSizes; j++) {
            tmp[i][j] = count(board, boardColSizes, *boardColSizes, i, j);
        }
    }
}
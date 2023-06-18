#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define BOARD_SIZE 8
#define CELL_WIDTH 4

int main() {
    initscr();
    curs_set(1);
    keypad(stdscr, TRUE);

    int board[BOARD_SIZE][BOARD_SIZE] = {0};  // 0: 빈 공간, 1: 흑돌, 2: 백돌

    // 오목판 그리기
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = j * CELL_WIDTH;
            int y = i * 2;

            mvprintw(y, x, "+---+");
            mvprintw(y + 1, x, "|   |");
            mvprintw(y + 2, x, "+---+");
        }
    }

    refresh();

    int row = 0, col = 0;
    int ch;
    while ((ch = getch()) != KEY_F(1)) {
        switch (ch) {
            case KEY_UP:
                row--;
                break;
            case KEY_DOWN:
                row++;
                break;
            case KEY_LEFT:
                col--;
                break;
            case KEY_RIGHT:
                col++;
                break;
            case '\n':
                if (board[row][col] == 0) {
                    // 선택한 위치에 동그라미 그리기
                    board[row][col] = 1;  // 흑돌
                    attron(COLOR_PAIR(1));  // 컬러 쌍 1 설정 (흑돌)
                    mvprintw(row * 2 + 1, col * CELL_WIDTH + 2, "O");
                    attroff(COLOR_PAIR(1));  // 컬러 쌍 1 해제
                }
                break;
        }

        // 좌표 범위 제한
        if (row < 0) row = 0;
        if (row >= BOARD_SIZE) row = BOARD_SIZE - 1;
        if (col < 0) col = 0;
        if (col >= BOARD_SIZE) col = BOARD_SIZE - 1;

        move(row * 2 + 1, col * CELL_WIDTH + 2);
        refresh();
    }

    endwin();
    return 0;
}
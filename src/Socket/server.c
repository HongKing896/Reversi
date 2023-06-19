#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 

#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <linux/socket.h>

#include <ncurses.h>

#define BOARD_SIZE 8
#define CELL_WIDTH 4
#define SERVER 1
#define CLIENT 2

enum Space{
	Empty,
	Black,
	White
};
void print_logo() {
	mvprintw(1 + 0, 35, "-------------------------------------------------------");
    mvprintw(1 + 1, 35, "|  ====    ==  ==      ==  =====  ====     ====    ==  |");
    mvprintw(1 + 2, 35, "|  == ==   ==  ==      ==  ==     == ==   ==   ==  ==  |");
    mvprintw(1 + 3, 35, "|  ====    ==   ==    ==   =====  ====      ===    ==  |");
    mvprintw(1 + 4, 35, "|  == ==   ==    ==  ==    ==     ==  ==  ==   ==  ==  |");
	mvprintw(1 + 5, 35, "|  ==  ==  ==      ==      =====  ==   ==   ====   ==  |");
	mvprintw(1 + 6, 35, "-------------------------------------------------------");
}

void print_turn() {
    mvprintw(10 + 0, 35, "   **     **  ***  ***    ****** **   ** *****  **     ** ");
    mvprintw(10 + 1, 35, "  ****   ****   ****        **   **   ** **  ** ** **  ** ");
    mvprintw(10 + 2, 35, " **  ** **  **   **         **   **   ** *****  **  ** ** ");
    mvprintw(10 + 3, 35, "**    **     **  **         **     ***   **  ** **     ** ");
}

void clear_print() {
    mvprintw(10 + 0, 35, "                                                            ");
    mvprintw(10 + 1, 35, "                                                            ");
    mvprintw(10 + 2, 35, "                                           	              ");
    mvprintw(10 + 3, 35, "                                                            ");
}

void print_win() {
    mvprintw(10 + 0, 35, "**    **     **  **  ****   **   ");
    mvprintw(10 + 1, 35, " **  ** **  **   **  ** **  **   ");
    mvprintw(10 + 2, 35, "  ****   ****    **  **  ** **   ");
    mvprintw(10 + 3, 35, "   **     **     **  **   ****   ");
}

void print_lose() {
    mvprintw(10 + 0, 35, "**      ****      ****    ****   ");
    mvprintw(10 + 1, 35, "**     **  **    **   **  **     ");
    mvprintw(10 + 2, 35, "**     **  **  **   **    **     ");
    mvprintw(10 + 3, 35, "******  ****     ****     ****   ");
}

void print_dr() {
    mvprintw(10 + 0, 35, " *****  *****     **   **    **     **  ");
    mvprintw(10 + 1, 35, " **  ** **  **   ****   **  ** **  **   ");
    mvprintw(10 + 2, 35, " **  ** *****   **  **   ****   ****    ");
    mvprintw(10 + 3, 35, " *****  **  ** **    **   **     **     ");
}

int listen_at_port (int portnum) 
{
	int sock_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (sock_fd == 0)  { 
		perror("socket failed : "); 
		exit(EXIT_FAILURE); 
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ; 
	bzero(&address, sizeof(address)) ; 	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY /* localhost */ ; 
	address.sin_port = htons(portnum); 

	if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed: "); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(sock_fd, 16 /* the size of waiting queue*/) < 0) { 
		perror("listen failed : "); 
		exit(EXIT_FAILURE); 
	} 

	int addrlen = sizeof(address); 
	int conn_fd = accept(sock_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
	if (conn_fd < 0) {
		perror("accept failed: "); 
		exit(EXIT_FAILURE); 
	}
	return conn_fd ;
}


// 주어진 위치에 돌을 놓을 수 있는지 확인
bool isValidMove(int row, int col, int board[BOARD_SIZE][BOARD_SIZE]) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != Empty) {
        return false;
    }
    
    // 주변 8방향을 확인하여 상대방 돌을 찾을 수 있는지 확인
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            int r = row + i;
            int c = col + j;
            bool foundOpponent = false;
            
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] == SERVER) {
                    if (foundOpponent) {
                        return true;
                    } else {
                        break;
                    }
                } else if (board[r][c] == Empty) {
                    break;
                } else {
                    foundOpponent = true;
                }
                
                r += i;
                c += j;
            }
        }
    }
    
    return false;
}

// 돌을 놓기
void makeMove(int row, int col,int board[BOARD_SIZE][BOARD_SIZE]) {
    board[row][col] = SERVER;
    
    // 돌을 뒤집을 수 있는 방향을 확인하여 뒤집기
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            int r = row + i;
            int c = col + j;
            bool foundOpponent = false;
            
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] == SERVER) {
                    if (foundOpponent) {
                        while (r != row || c != col) {
                            r -= i;
                            c -= j;
                            board[r][c] = SERVER;
                        }
                    }
                    
                    break;
                } else if (board[r][c] == Empty) {
                    break;
                } else {
                    foundOpponent = true;
                }
                
                r += i;
                c += j;
            }
        }
    }
}

void print_move(int board[BOARD_SIZE][BOARD_SIZE]){
	int row = 0, col = 0, turn = SERVER;
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
				if(!isValidMove(row,col,board)) break;
				if (board[row][col] == 0) {
					// 선택한 위치에 동그라미 그리기
					if (turn == SERVER) {
						board[row][col] = 1;  // 흑돌
						attron(COLOR_PAIR(1));  // 컬러 쌍 1 설정 (흑돌)
						mvaddch(row * 2 + 1, col * CELL_WIDTH + 2, 'X');
						attroff(COLOR_PAIR(1));  // 컬러 쌍 1 해제
						makeMove(row,col,board);
					} else if (turn == CLIENT) {
						board[row][col] = 2;  // 흰돌
						attron(COLOR_PAIR(2));  // 컬러 쌍 2 설정 (흰돌)
						mvaddch(row * 2 + 1, col * CELL_WIDTH + 2, 'O');
						attroff(COLOR_PAIR(2));  // 컬러 쌍 2 해제
						makeMove(row,col,board);
					}
				}
				refresh();
				return;
		}

		// 좌표 범위 제한
		if (row < 0) row = 0;
		if (row >= BOARD_SIZE) row = BOARD_SIZE - 1;
		if (col < 0) col = 0;
		if (col >= BOARD_SIZE) col = BOARD_SIZE - 1;

		move(row * 2 + 1, col * CELL_WIDTH + 2);
		refresh();
	}
}

// 게임 종료 여부 확인
bool isGameOver(int board[BOARD_SIZE][BOARD_SIZE]) {
    int blackCount = 0;
    int whiteCount = 0;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == Black) {
                blackCount++;
            } else if (board[i][j] == White) {
                whiteCount++;
            }
        }
    }
    
    return (blackCount + whiteCount) == BOARD_SIZE * BOARD_SIZE || blackCount == 0 || whiteCount == 0;
}

void print_board(int board[BOARD_SIZE][BOARD_SIZE]){
	print_logo();
	for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = j * CELL_WIDTH;
            int y = i * 2;

            mvprintw(y, x, "+---+");
            mvprintw(y + 1, x, "|   |");
            mvprintw(y + 2, x, "+---+");
			
			if(board[i][j] == Black){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| O |");
				mvprintw(y + 2, x, "+---+");
			}
			if(board[i][j] == White){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| X |");
				mvprintw(y + 2, x, "+---+");
			}
			if(isValidMove(i,j,board)){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| . |");
				mvprintw(y + 2, x, "+---+");
			}
        }
    }
	refresh();
}
void play_game (int conn_fd,int board[BOARD_SIZE][BOARD_SIZE]) 
{
	initscr();
    curs_set(1);
    keypad(stdscr, TRUE);
	int count = 0;
	do {
		int s ;
		
		while ((s = recv(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0)) == 0) ;
		if (s == -1)
			break ;

		print_board(board);

		// Notice Order
		print_turn();
		
	
		print_move(board);

		send(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0);

		clear_print();

		print_board(board);

	//} while (!isGameOver(board)) ;
	count ++; 
	} while (count < 4) ;
	int blackCount = 0;
    int whiteCount = 0;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == Black) {
                blackCount++;
            } else if (board[i][j] == White) {
                whiteCount++;
            }
        }
    }
	
	clear_print();

	if(blackCount > whiteCount) 
		print_win();
	else if (blackCount < whiteCount) 
		print_lose();
	else 
		print_dr();

	int check;
	while((check= getch()) != KEY_ENTER) {
		switch (check) {
			case '\n':
				endwin();
				break;
		}
	}
	return;

}

int main (int argc, char const **argv) 
{	
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments!\n") ;
		exit(EXIT_FAILURE) ;
	}

	int conn_fd = listen_at_port(atoi(argv[1])) ;

	int board[BOARD_SIZE][BOARD_SIZE];

	play_game(conn_fd,board) ;

	shutdown(conn_fd, SHUT_RDWR) ;

	return EXIT_SUCCESS ;
} 


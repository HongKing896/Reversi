#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
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
    mvprintw(5 + 0, 35, " ||===   ||  ||	|| ||===  ||===    ====	   || ");
    mvprintw(5 + 1, 35, " ||  ==  ||  ||	|| ||	  ||  ==  ==   ==  || ");
    mvprintw(5 + 2, 35, " ||===   ||   ||  ||  ||===  ||===	   ===	   || ");
    mvprintw(5 + 3, 35, " || ||	  ||	||||   ||	  || ||   ==   ==  || ");
	mvprintw(5 + 3, 35, " ||  ||  ||	 ||	   ||===  ||  ||    ====   || ");
}

void print_turn() {
    mvprintw(5 + 0, 35, "   **     **  ***  ***  ****** **   ** *****  **     ** ");
    mvprintw(5 + 1, 35, "  ****   ****   ****      **   **   ** **  ** ** **  ** ");
    mvprintw(5 + 2, 35, " **  ** **  **   **       **   **   ** *****  **  ** ** ");
    mvprintw(5 + 3, 35, "**    **     **  **       **     ***   **  ** **     ** ");
}

void clear_print_turn() {
    mvprintw(5 + 0, 35, "                                                           ");
    mvprintw(5 + 1, 35, "                                                           ");
    mvprintw(5 + 2, 35, "                                           	            ");
    mvprintw(5 + 3, 35, "                                                           ");
}

void print_win() {
    mvprintw(5 + 0, 35, "**    **     **  **  ****   **   ");
    mvprintw(5 + 1, 35, " **  ** **  **   **  ** **  **   ");
    mvprintw(5 + 2, 35, "  ****   ****    **  **  ** **   ");
    mvprintw(5 + 3, 35, "   **     **     **  **   ****   ");
}

<<<<<<< HEAD
void print_lose() {
    mvprintw(5 + 0, 35, "**      ****      ****    ****   ");
    mvprintw(5 + 1, 35, "**     **  **    **   **  **     ");
    mvprintw(5 + 2, 35, "**     **  **  **   **    **     ");
    mvprintw(5 + 3, 35, "******  ****     ****     ****   ");
}

void print_dr() {
    mvprintw(5 + 0, 35, " *****  *****     **   **    **     **  ");
    mvprintw(5 + 1, 35, " **  ** **  **   ****   **  ** **  **   ");
    mvprintw(5 + 2, 35, " **  ** *****   **  **   ****   ****    ");
    mvprintw(5 + 3, 35, " *****  **  ** **    **   **     **     ");
}

void print_board(int board[BOARD_SIZE][BOARD_SIZE]){	
	for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = j * CELL_WIDTH;
            int y = i * 2;

            mvprintw(y, x, "+---+");
            mvprintw(y + 1, x, "|   |");
            mvprintw(y + 2, x, "+---+");

			if(board[i][j] == 1){
				 int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| X |");
				mvprintw(y + 2, x, "+---+");
			}
			if(board[i][j] == 2){
				 int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| O |");
				mvprintw(y + 2, x, "+---+");
			}
        }
    }
	refresh();
}

=======
>>>>>>> d392e0fbf2ef0097d2a7cac4d8f820a2f20b2851
int connect_ipaddr_port (const char * ip, int port)
{
	int sock_fd ;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	int opt = 2 ;
	if (setsockopt(sock_fd, SOL_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
		fprintf(stderr, "fail at setsockopt\n") ;
		exit(EXIT_FAILURE) ;
	}

	struct sockaddr_in address ;
	bzero(&address, sizeof(address)) ;
	address.sin_family = AF_INET ; 
	address.sin_port = htons(port) ; 
	if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
		perror("inet_pton failed : ") ; 
		exit(EXIT_FAILURE) ;
	} 

	if (connect(sock_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}
	return sock_fd ;
}

void init_board(int board[BOARD_SIZE][BOARD_SIZE]){
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = Empty;
        }
    }
    
    board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2 - 1] = White;
    board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2] = Black;
    board[BOARD_SIZE / 2][BOARD_SIZE / 2 - 1] = Black;
    board[BOARD_SIZE / 2][BOARD_SIZE / 2] = White;
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
                if (board[r][c] == CLIENT) {
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
    board[row][col] = CLIENT;
    
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
                if (board[r][c] == CLIENT) {
                    if (foundOpponent) {
                        while (r != row || c != col) {
                            r -= i;
                            c -= j;
                            board[r][c] = CLIENT;
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
	int row = 0, col = 0, turn = CLIENT;
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
	for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = j * CELL_WIDTH;
            int y = i * 2;

            mvprintw(y, x, "+---+");
            mvprintw(y + 1, x, "|   |");
            mvprintw(y + 2, x, "+---+");
			
			if(board[i][j] == 1){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| X |");
				mvprintw(y + 2, x, "+---+");
			}
			if(board[i][j] == 2){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| O |");
				mvprintw(y + 2, x, "+---+");
			}
			if(isValidMove(i,j,board)){
				int x = j * CELL_WIDTH;
				int y = i * 2;

				mvprintw(y, x, "+---+");
				mvprintw(y + 1, x, "| + |");
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

	do {
		print_board(board);

		// Notice Order
		print_turn();

		check_board(board);
		print_move(board);

		send(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0);

		clear_print_turn();

		print_board(board);

		int s ;
		while ((s = recv(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0)) == 0) ;
		if (s == -1)
			break ;

		print_board(board);
		
	} while (!isGameOver(board)) ;

	  endwin();
}

int main (int argc, char const ** argv)
{ 
	struct sockaddr_in serv_addr; 
	int sock_fd ;
	int s, len ;
	int board[BOARD_SIZE][BOARD_SIZE]; 
	char * data ;

	if (argc != 3) {
		fprintf(stderr, "Wrong number of arguments\n");
		fprintf(stderr, "Usage: ./chat-sendfirst [IP addr] [Port num]\n") ;
		exit(EXIT_FAILURE) ;
	}

	int conn_fd = connect_ipaddr_port(argv[1], atoi(argv[2])) ;
	
	init_board(board);

	play_game(conn_fd,board);

	shutdown(conn_fd, SHUT_RDWR) ;

	return EXIT_SUCCESS ;
} 
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

enum Space{
	Empty,
	Black,
	White
};

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


void check_board(int board[BOARD_SIZE][BOARD_SIZE]){
	int row = 0, col = 0, turn = 2;
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
					if (turn == 1) {
						board[row][col] = 1;  // 흑돌
						attron(COLOR_PAIR(1));  // 컬러 쌍 1 설정 (흑돌)
						mvaddch(row * 2 + 1, col * CELL_WIDTH + 2, 'X');
						attroff(COLOR_PAIR(1));  // 컬러 쌍 1 해제
					} else if (turn == 2) {
						board[row][col] = 2;  // 흰돌
						attron(COLOR_PAIR(2));  // 컬러 쌍 2 설정 (흰돌)
						mvaddch(row * 2 + 1, col * CELL_WIDTH + 2, 'O');
						attroff(COLOR_PAIR(2));  // 컬러 쌍 2 해제
					}
					// attroff(COLOR_PAIR(1));  // 컬러 쌍 1 해제
					// attroff(COLOR_PAIR(2));  // 컬러 쌍 2 해제
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

void chat (int conn_fd,int board[BOARD_SIZE][BOARD_SIZE]) 
{
	//char buf[256] ;
	initscr();
    curs_set(1);
    keypad(stdscr, TRUE);
	int count = 0;
	do {
		int s ;
		
		//while ( (s = recv(conn_fd, buf, 255, 0)) == 0 ) ;
		while ((s = recv(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0)) == 0) ;
		if (s == -1)
			break ;

		//buf[s] = '\0' ;
		//printf(">%s\n", buf) ;
		print_board(board);
		

		//fgets(buf, 256, stdin) ;
		//buf[strlen(buf) - 1] = '\0' ;
		//if (strcmp(buf, "quit()") == 0)
		//	break ;
		check_board(board);

		//send(conn_fd, buf, strlen(buf), 0) ;
		send(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0);

		print_board(board);

		count++;
	} while (count < 6) ;
}

int main (int argc, char const **argv) 
{	
	if (argc != 2) {
		fprintf(stderr, "Wrong number of arguments!\n") ;
		exit(EXIT_FAILURE) ;
	}

	int conn_fd = listen_at_port(atoi(argv[1])) ;

	int board[BOARD_SIZE][BOARD_SIZE];
	//init_board(board);

	chat(conn_fd,board) ;

	shutdown(conn_fd, SHUT_RDWR) ;
} 


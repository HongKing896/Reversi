#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <linux/socket.h>

#define BOARD_SIZE 8
enum Space{
	Empty,
	Black,
	White
};

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
        for (int j = 0; j < BOARD_SIZE; j++){
			board[i][j] = Empty;
			if(i == 3){
				if(j == 3) board[i][j] = White;
				if(j == 4) board[i][j] = Black;
			}
			if(i == 4){
				if(j == 3) board[i][j] = Black;
				if(j == 4) board[i][j] = White;
			}
        }
    }
}

void print_board(int board[BOARD_SIZE][BOARD_SIZE]){
	for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++){
            printf("%d ",board[i][j]);
        }
		printf("\n");
	}	
}

// void send_board(int board[BOARD_SIZE][BOARD_SIZE],int conn_fd){
// 	send(conn_fd, board, strlen(board), 0);
// }
// void recv_board(){

// }
void chat (int conn_fd,int board[BOARD_SIZE][BOARD_SIZE])
{
	char buf[256] ;

	do {
		fgets(buf, 256, stdin) ;
		buf[strlen(buf) - 1] = '\0' ;
		if (strcmp(buf, "quit()") == 0)
			break ;

		//send(conn_fd, buf, strlen(buf), 0) ;
		send(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0);

		int s ;
		//while ((s = recv(conn_fd, buf, 1024, 0)) == 0) ;
		while ((s = recv(conn_fd, board, sizeof(int)*BOARD_SIZE*BOARD_SIZE, 0)) == 0) ;
		if (s == -1)
			break ;
		//buf[s] = '\0' ;

		//printf(">%s\n", buf) ;
		print_board(board);
	} while (strcmp(buf, "quit()") != 0) ;
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
	
	chat(conn_fd,board);

	shutdown(conn_fd, SHUT_RDWR) ;

	return EXIT_SUCCESS ;
} 
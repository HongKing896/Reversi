.PHONY : all
all : server client

.PHONY : clean
clean:
	rm -rf ../../bin/*

server : server.c
	gcc ./server.c  -o ../../bin/server -lncurses

client : client.c
	gcc ./client.c  -o ../../bin/client -lncurses
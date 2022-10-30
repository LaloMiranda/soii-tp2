CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all: clean bdd server

server:
	$(CC) $(CFLAGS) ./src/server.c ./obj/bdd.obj -o ./bin/server.out -lrt -lsqlite3

bdd:
	$(CC) $(CFLAGS) -c ./src/bdd.c  -o ./obj/bdd.obj -lrt -lsqlite3

clean:
	rm -f ./bin/*
	rm -f ./obj/*
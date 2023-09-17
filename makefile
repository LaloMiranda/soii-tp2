CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all: clean bdd sockets server

server:
	$(CC) $(CFLAGS) ./src/server.c ./obj/bdd.obj ./obj/sockets.obj -o ./bin/server.out -lrt -lsqlite3

bdd:
	$(CC) $(CFLAGS) -c ./src/bdd.c  -o ./obj/bdd.obj -lrt -lsqlite3

sockets:
	$(CC) $(CFLAGS) -c ./src/sockets.c  -o ./obj/sockets.obj -lrt

clientes: obj/cliente-db.o cliente_continuo cliente_query cliente_descarga


obj/cliente-db.o:
	$(CC) $(CFLAGS) -c ./src/cliente-db.c -o obj/cliente-db.o

cliente_continuo:
	$(CC) $(CFLAGS) ./src/cliente_continuo.c obj/cliente-db.o -o ./bin/cliente_continuo.out -lrt

cliente_query:
	$(CC) $(CFLAGS) ./src/cliente_query.c obj/cliente-db.o -o ./bin/cliente_query.out -lrt

cliente_descarga:
	$(CC) $(CFLAGS) ./src/cliente_descarga.c obj/cliente-db.o -o ./bin/cliente_descarga.out -lrt

clean:
	rm -f ./bin/*
	rm -f ./obj/*
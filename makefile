CC=gcc
CFLAGS= -g -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all: clean bin/server clientes

bin/server: obj/dependencias.o obj/baseDatos.o
	$(CC) $(CFLAGS) ./src/server.c obj/dependencias.o obj/baseDatos.o -o ./bin/server.out -lrt -lsqlite3

obj/dependencias.o:
	$(CC) $(CFLAGS) -c ./src/dependencias.c -o obj/dependencias.o -lsqlite3

obj/baseDatos.o: obj/dependencias.o
	$(CC) $(CFLAGS) -c ./src/baseDatos.c -o obj/baseDatos.o -lsqlite3

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
#ifndef _DEPENDENCIAS_
#define _DEPENDENCIAS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sqlite3.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define BUF_SIZE 64000
#define MAX_CLIENT 1000

typedef struct{
	sqlite3 *conexiones[5];
	int indice;

} BaseDeDatos;

union semun{
	int val;
	struct semaforo_ds *buf;
	unsigned short *array;
};

//Inicializacion del server
BaseDeDatos *crearMemComp();
void	crearSockets(uint16_t puerto1, uint16_t puerto2, char *path, BaseDeDatos *db, int semaforo);

// Gestoin de semaforo
void  configSemaforo(int semaforo, int num);
int   crearSemaforo();
void  borrarSemaforo(int semaforo);
void  decreSem(int semaforo);
void  increSem(int semaforo);

// Configuracion de sockets
int   configSockerIPv4(uint16_t puerto);
int   configSockerIPv6(uint16_t puerto);
void  configSocketUnix(char *path, BaseDeDatos *db, int semaforo);
void  serverIPv4forks (uint16_t puerto, BaseDeDatos *db, int semaforo);
void  serverIPv6forks (uint16_t puerto, BaseDeDatos *db, int semaforo);

// Cierre de la app
void  borrarMemcomp(int shmid);
void  regIntGlob(int val, int ind);
void  cerrarServer();

#endif
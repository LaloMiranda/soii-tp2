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
	sqlite3 *arreglo[5];
	int indice;

} BasesDatos;

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int valGlob[5];

// Gestoin de semaforo
void  configSemaforo(int semid, int num);
int   crearSemaforo();
void  borrarSemaforo(int semid);
void  decreSem(int semid);
void  increSem(int semid);

// Configuracion de sockets
int   configSockerIPv4(uint16_t puerto);
int   configSockerIPv6(uint16_t puerto);
void  configSocketUnix(char *path, BasesDatos *db, int semid);
void  serverIPv4forks (uint16_t puerto, BasesDatos *db, int semid);
void  serverIPv6forks (uint16_t puerto, BasesDatos *db, int semid);

// Cierre de la app
void  borrarMemcomp(int shmid);
void  regIntGlob(int val, int ind);
void  cerrarServer();

#endif
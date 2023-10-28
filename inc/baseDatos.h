#ifndef _BASEDATOS_
#define _BASEDATOS_

#include "../inc/dependencias.h"
#include <sqlite3.h>

// Gestion e incio de la DB
void abrirConexionesDB(BaseDeDatos *db);
void crearBaseDatos(void);

// Envio de la DB completa
void sendBaseDatos(int socketCli);

// Query y loggeo de pedidos
char *getQueryBaseDatos(char *query, BaseDeDatos *db, int protocolo, int semaforo);
char *logearClienteBaseDatos(char *command, int protocolo);
int obtenerIndice(BaseDeDatos *db);  
int callback(void *NotUsed, int cantColumnas, char **valCell, char **nameCol);

char result[BUF_SIZE];
#endif
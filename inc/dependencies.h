#ifndef _DEPENDENCIES_
#define _DEPENDENCIES_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//Bdd
#include <sqlite3.h>
#include <sys/shm.h>

typedef struct
{
	sqlite3 *conecciones[5];
	int indice;

} BaseDeDatos;


//Funciones relacionadas a Bdds

BaseDeDatos* crearMemBdd();
void crearBdds();
void abrirConexionesBdd(BaseDeDatos *db);

#endif
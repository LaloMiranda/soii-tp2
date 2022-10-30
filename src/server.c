#include "../inc/dependencies.h"

int main(int argc, char const *argv[]){
    if (argc != 4){
        printf("Formato de enrtada: %s <portIPv4> <portIPv6> <pathUnix>\n", argv[0]);
        exit(0);
    }

    //Creo la memoria compartida
    BaseDeDatos *bdd = crearMemBdd();

    //Creo y cargo las bdds (Peliculas y lgos)
    crearBdds();

    //Abro las conexiones de la bdd
    abrirConexionesBdd(bdd);

    return 0;
}


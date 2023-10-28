#include "../inc/baseDatos.h"

int main(int argc, char *argv[]){
  if (argc != 4){
    fprintf(stderr, "You should enter: %s <portIPv4> <portIPv6> <pathUnix>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  //Creo la DB y la memoria compartida
  BaseDeDatos *db;
  db = crearMemComp();

  //Creo el semaforo que va a gestionar los accesos a la DB
  int semaforo = crearSemaforo();;
  configSemaforo(semaforo, 1);
  regIntGlob(semaforo, 4);

  //Populo la DB y habilito las conexiones
  crearBaseDatos();
  abrirConexionesDB(db);

  printf("[SERVER] Puerto para IPv4 es: %s\n[SERVER] Puerto para IPv6 es: %s\n", argv[1], argv[2]);

  //Habilito las conexiones IPv4, IPv6 y Unix
  crearSockets((uint16_t)atoi(argv[1]), (uint16_t)atoi(argv[2]), argv[3], db, semaforo);
  

  while (1);

  return 0;
}
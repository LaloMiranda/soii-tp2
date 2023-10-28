#include "../inc/baseDatos.h"

int pid, pid1, pid2;
BasesDatos *crearMemComp()
{
  BasesDatos *reg;
  int mem_comp = shmget(ftok("./memComp", 'S'), sizeof(reg), (IPC_CREAT | 0660));

  if (mem_comp < 0)
  {
    perror("[SERVER] Error al crear mem_omp\n");
    exit(EXIT_FAILURE);
  }
  regIntGlob(mem_comp, 3);
  reg = shmat(mem_comp, 0, 0);
  if (reg == (void *)-1)
  {
    perror("[SERVER] Error al asignar memoria\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("[SERVER] Se creo memoria compartida\n");
  }
  return reg;
}

void crearHijos(uint16_t puerto1, uint16_t puerto2, char *path, BasesDatos *db, int semid)
{
  pid = fork();
  regIntGlob(pid, 0);
  if (pid < 0)
  {
    perror("Error al crear el hijo1\n");
    exit(EXIT_FAILURE);
  }
  else if (pid == 0)
  {
    serverIPv4forks(puerto1, db, semid);
    exit(1);
  }

  pid1 = fork();
  regIntGlob(pid1, 1);
  if (pid1 < 0)
  {

    perror("Error al crear el hijo2\n");
    exit(EXIT_FAILURE);
  }
  else if (pid1 == 0)
  {
    serverIPv6forks(puerto2, db, semid);
    exit(1);
  }
  pid2 = fork();
  regIntGlob(pid2, 2);
  if (pid2 < 0)
  {
    printf("Error al crear el hijo");
    exit(EXIT_FAILURE);
  }
  else if (pid2 == 0)
  {
    configSocketUnix(path, db, semid);
    exit(1);
    // serverUnixStart(puerto);
  }
  signal(SIGINT, cerrarServer);
}

int main(int argc, char *argv[]){
  if (argc != 4){
    fprintf(stderr, "You should enter: %s <portIPv4> <portIPv6> <pathUnix>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  //Creo la DB y la memoria compartida
  BasesDatos *db;
  db = crearMemComp();

  //Creo el semaforo que va a gestionar los accesos a la DB
  int semid;
  semid = crearSemaforo();
  configSemaforo(semid, 1);
  regIntGlob(semid, 4);

  //Populo la DB y habilito las conexiones
  crearBaseDatos();
  abrirConexionesDB(db);

  printf("[SERVER] Puerto para IPv4 es: %s\n[SERVER] Puerto para IPv6 es: %s\n", argv[1], argv[2]);

  //Habilito las conexiones IPv4, IPv6 y Unix
  crearHijos((uint16_t)atoi(argv[1]), (uint16_t)atoi(argv[2]), argv[3], db, semid);
  

  while (1);

  return 0;
}
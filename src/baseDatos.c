#include "../inc/baseDatos.h"

/**
 * @brief Abro las conexiones de la DB con maximo 5
 * @param db 
 */
void abrirConexionesDB(BasesDatos *db){
  for (int i = 0; i < 5; i++){
    int rc = sqlite3_open("./bin/AnimeList.db", &db->arreglo[i]);
    if (rc != SQLITE_OK){
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db->arreglo[i]));
      sqlite3_close(db->arreglo[i]);
      exit(EXIT_FAILURE);
    }

    sqlite3_busy_timeout(db->arreglo[i], 1000);
  }
  fprintf(stdout, "[SERVER] Abri las conexiones a la DB\n");
  db->indice = 0;
}

/**
 * @brief Genero la DB y los datos que va a tener
 * 
 */
void crearBaseDatos(void){
  sqlite3 *db;
  char *err_msg = 0;

  int rc = sqlite3_open("./bin/AnimeList.db", &db);
  if (rc != SQLITE_OK){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  char *sql = "DROP TABLE IF EXISTS Animes;"
              "CREATE TABLE Animes(Id INTEGER PRIMARY KEY, Name TEXT, Date_Release INT ,Caps INT);"
              "INSERT INTO Animes VALUES(1, 'Fullmetal Alchemist', 2009, 64);"
              "INSERT INTO Animes VALUES(2, 'Shingeki no Kyojin', 2013, 60);"
              "INSERT INTO Animes VALUES(3, 'Hunter x Hunter', 2011, 126);"
              "INSERT INTO Animes VALUES(4, 'One Piece', 1999, 1013);"
              "INSERT INTO Animes VALUES(5, 'Dragon Ball', 1986, 600);"
              "INSERT INTO Animes VALUES(6, 'Sailor Moon', 1992, 46);"
              "INSERT INTO Animes VALUES(7, 'Naruto', 2002, 218);"
              "INSERT INTO Animes VALUES(8, 'Slam Dunk', 1993, 101);"
              "INSERT INTO Animes VALUES(9, 'Jujutsu Kaisen', 2020, 24);"
              "INSERT INTO Animes VALUES(10, 'Death Note', 2006, 37);"
              "INSERT INTO Animes VALUES(11, 'Yū Yū Hakusho', 1992, 112);"
              "INSERT INTO Animes VALUES(12, 'Himouto! Umaru-chan', 2013, 24);";
  char *sq2 = "DROP TABLE IF EXISTS Log;"
              "CREATE TABLE Log(PID TEXT, PROTOCOLO TEXT, CMD TEXT, TIME TEXT);";

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  if (rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  rc = sqlite3_exec(db, sq2, 0, 0, &err_msg);
  if (rc != SQLITE_OK){
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "[SERVER] Base de Datos Creada\n");
  sqlite3_close(db);
}

// FUNCION PARA ENVIAR LA DB POR COMPLETO AL CLIENTE

/**
 * @brief Devolver base de datos al cliente
 * 
 * @param socketCli 
 */
void sendBaseDatos(int socketCli){
  FILE *fp = fopen("./bin/AnimeList.db", "rb");
  if (fp == NULL){
    perror("Error al abrir el archivo de la DB para enviarlo a cliente en sendBaseDatos()\n");
    exit(EXIT_FAILURE);
  }

  while (1){
    unsigned char buff[256] = {0};
    int nread = (int)fread(buff, 1, 256, fp);

    if (nread > 0){               //Si es mayor a 0 lei datos correctamente
      write(socketCli, buff, (size_t)nread);
    }

    if (nread < 256){             //Si es menor a 256 valido que el archivo este bien
      if(ferror(fp)){
        printf("Error de lectura sobre el archivo de la DB en sendBaseDatos()\n");
      }
      break;
    }
  }

  close(socketCli);
}

// FUNCIONES PARA EL ENVIO DE UNA QUERY SOBRE LA DB

/**
 * @brief Hago la query pedida por el cliente sobre la DB y loggeo el resultado
 * 
 * @param query Pedido del cliente
 * @param db Base de datos
 * @param protocolo 1: IPv4 - 2: IPv6 - 3: UNIX 
 * @param semid ID del semaforo
 * @return char* 
 */
char *getQueryBaseDatos(char *query, BasesDatos *db, int protocolo, int semid){
  char *err_msg = 0;
  int rc;
  

  decreSem(semid);
  int i = obtenerIndice(db);
  increSem(semid);

  char *queryLog;
  queryLog = logearClienteBaseDatos(query, protocolo);
  rc = sqlite3_exec(db->arreglo[i], queryLog, 0, 0, &err_msg);

  char aux[50];
  if (rc != SQLITE_OK){
      fprintf(stderr, "Failed to select data\n");
      fprintf(stderr, "SQL error: %s\n", err_msg);
      printf("ACA ES EL ERROR\n");
      sqlite3_free(err_msg);
  }
  if (protocolo != 3){
    rc = sqlite3_exec(db->arreglo[i], query, callback, 0, &err_msg);
    if (rc != SQLITE_OK){
      memset(aux, 0, sizeof(aux));
      //fprintf(stderr, "Failed to select data\n");
      sprintf(aux, "SQL error: %s\n", err_msg);
      sqlite3_free(err_msg);
      return strdup(aux);
    }
  }

  return result;
}

/**
 * @brief Genero string para guarda en la tabla de Logs
 * 
 * @param command 
 * @param protocolo 
 * @return char* 
 */
char *logearClienteBaseDatos(char *command, int protocolo){
  time_t t;
  struct tm *tm;
  t = time(NULL);
  tm = localtime(&t);

  char fyh[BUF_SIZE];
  strftime(fyh, BUF_SIZE, "%d/%m/%Y - %H:%M:%S", tm);

  char *aux = malloc(sizeof(char) * BUF_SIZE);
  char* prot;
  if (protocolo == 1){
    prot = "IPv4";
  }
  else if (protocolo == 2){
    prot = "IPv6";
  }
  else {
    prot = "UNIX";
  }
  sprintf(aux, "INSERT INTO LOG VALUES(\"%d\",\"%s\",\"%s\",\"%s\");", getpid(), prot, command, fyh);
  return aux;
}

/**
 * @brief Devuelvo el indice de la DB
 * 
 * @param db 
 * @return int 
 */
int obtenerIndice(BasesDatos *db){
  int ind = db->indice;
  db->indice++;
  db->indice %= 5;
  return ind;
}

/**
 * @brief Funcion de callback para getQueryBaseDatos
 * 
 * @param NotUsed 
 * @param cantColumnas 
 * @param valCell 
 * @param nameCol 
 * @return int 
 */
int callback(void *NotUsed, int cantColumnas, char **valCell, char **nameCol){
  (void)NotUsed;
  char aux[BUF_SIZE];

  for (int i = 0; i < cantColumnas; i++){
    if (sprintf(aux, "%s: %s\n", nameCol[i], valCell[i]) < i){
        perror("Error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }
    strcat(result, aux);
    memset(aux, 0, BUF_SIZE);
  }
  strcat(result, "\n");
  return 0;
}
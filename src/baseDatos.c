#include "../inc/baseDatos.h"

/**
 * @brief Abro las conexiones de la DB con maximo 5
 * @param db 
 */
void abrirConexionesDB(BaseDeDatos *db){
  for (int i = 0; i < 5; i++){
    int rc = sqlite3_open("./bin/Pelis.db", &db->conexiones[i]);
    if (rc != SQLITE_OK){
      fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db->conexiones[i]));
      sqlite3_close(db->conexiones[i]);
      exit(EXIT_FAILURE);
    }

    sqlite3_busy_timeout(db->conexiones[i], 1000);
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

  int rc = sqlite3_open("./bin/Pelis.db", &db);
  if (rc != SQLITE_OK){
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  char *sql = "DROP TABLE IF EXISTS Pelis;"
              "CREATE TABLE Pelis(Id INTEGER PRIMARY KEY, Name TEXT, Date_Release INT ,Lenght INT);"
              "INSERT INTO Pelis VALUES(1, 'The Shawshank Redemption',  1994, 142);"
              "INSERT INTO Pelis VALUES(2, 'The Godfather',             1972, 175);"
              "INSERT INTO Pelis VALUES(3, 'The Dark Knight',           2008, 152);"
              "INSERT INTO Pelis VALUES(4, 'The Godfather 2',           1974, 202);"
              "INSERT INTO Pelis VALUES(5, 'Angry Men',                 1957, 96);"
              "INSERT INTO Pelis VALUES(6, 'Schlinders list',           1993, 195);"
              "INSERT INTO Pelis VALUES(7, 'The Lord of The Rings 2',   2003, 201);"
              "INSERT INTO Pelis VALUES(8, 'Pulp Fiction',              1994, 154);"
              "INSERT INTO Pelis VALUES(9, 'The Lord of The Rings 1',   2001, 178);"
              "INSERT INTO Pelis VALUES(10, 'Forrest Gump',             1994, 142);";
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
  FILE *fp = fopen("./bin/Pelis.db", "rb");
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
 * @param semaforo ID del semaforo
 * @return char* 
 */
char *getQueryBaseDatos(char *query, BaseDeDatos *db, int protocolo, int semaforo){
  char *err_msg = 0;
  int rc;

  decreSem(semaforo);
  int i = obtenerIndice(db);
  increSem(semaforo);

  char *queryLog;
  queryLog = logearClienteBaseDatos(query, protocolo);
  rc = sqlite3_exec(db->conexiones[i], queryLog, 0, 0, &err_msg);

  char aux[50];
  if (rc != SQLITE_OK){
      fprintf(stderr, "Failed to select data\n");
      fprintf(stderr, "SQL error: %s\n", err_msg);
      printf("ACA ES EL ERROR\n");
      sqlite3_free(err_msg);
  }
  if (protocolo != 3){
    rc = sqlite3_exec(db->conexiones[i], query, callback, 0, &err_msg);
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
  char *protocolos[3] = {"IPv4", "IPv6", "UNIX"};

  time_t t;
  struct tm *tm;
  t = time(NULL);
  tm = localtime(&t);

  char fyh[BUF_SIZE];
  strftime(fyh, BUF_SIZE, "%d/%m/%Y - %H:%M:%S", tm);

  char *aux = malloc(sizeof(char) * BUF_SIZE);
  sprintf(aux, "INSERT INTO LOG VALUES(\"%d\",\"%s\",\"%s\",\"%s\");", getpid(), protocolos[protocolo], command, fyh);
  return aux;
}

/**
 * @brief Devuelvo el indice de la DB
 * 
 * @param db 
 * @return int 
 */
int obtenerIndice(BaseDeDatos *db){
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
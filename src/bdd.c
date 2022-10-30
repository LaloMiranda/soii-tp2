#include "../inc/dependencies.h"

/**
 * @brief Creo la memoria compartida a usar por la bdd
 * 
 * @return BaseDeDatos* 
 */
BaseDeDatos* crearMemBdd(){
    BaseDeDatos *bdd;

    key_t key = ftok("./README.md", 'S');
    if(key < 0){
        printf("Error en ftok, crearMemBdd()\n");
    }

    int mem_comp = shmget(key, sizeof(bdd), (IPC_CREAT | 0660));
    if (mem_comp < 0){
        printf("Error al crearla memoria compartida, crearMemBdd()\n");
        exit(1);
    }

    bdd = shmat(mem_comp, 0, 0);
    if (bdd == (void *) - 1){
        perror("Error al asignar memoria, crearMemBdd()\n");
        exit(1);
    }
    return bdd;
}

/**
 * @brief Creo una bdd y la cargo con datos
 * 
 */
void crearBdds(){
    sqlite3 *db;
    char *err_msg = 0;
    int rc;

    rc = sqlite3_open("./bin/Peliculas.db", &db);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error al abrir la bdd, crearBaseDatos(): %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    char *sql = "DROP TABLE IF EXISTS Peliculas;"
                "CREATE TABLE Peliculas(Id INTEGER PRIMARY KEY, Nombre TEXT, Año_de_salida INT , Duracion INT);"
                "INSERT INTO Peliculas VALUES(1, 'Cadena perpetua', 1994, 142);"
                "INSERT INTO Peliculas VALUES(2, 'El padrino', 1972, 175);"
                "INSERT INTO Peliculas VALUES(3, 'Batman: El caballero de la noche', 2008, 152);";

    char *sq2 = "DROP TABLE IF EXISTS Log;"
                "CREATE TABLE Log(PID TEXT, PROTOCOLO TEXT, CMD TEXT, TIME TEXT);";

    //Creo la bdd de peliculas
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK){

        fprintf(stderr, "Error al operar sobre la bdd, crearBaseDatos() -> Peliculas: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }

    //Creo la bdd con los logs
    rc = sqlite3_exec(db, sq2, 0, 0, &err_msg);
    if (rc != SQLITE_OK){
        fprintf(stderr, "Error al operar sobre la bdd, crearBaseDatos() -> Logs: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_close(db);
}

/**
 * @brief Cargo la bdd dentro de la estructura de bdds
 * 
 * @param db 
 */
void abrirConexionesBdd(BaseDeDatos *db){
    for (int i = 0; i < 5; i++){
        int rc = sqlite3_open("./bin/Peliculas.db", &db->conecciones[i]);
        if (rc != SQLITE_OK){
            fprintf(stderr, "Error al abrir la conexion %i: %s\n", i, sqlite3_errmsg(db->conecciones[i]));
            sqlite3_close(db->conecciones[i]);
            exit(1);
        }

        sqlite3_busy_timeout(db->conecciones[i], 1000);
    }
    db->indice = 0;
}
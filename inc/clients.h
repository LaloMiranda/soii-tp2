#ifndef _CLIENTS_
#define _CLIENTS_

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <signal.h>

// Max message size
#define BUF_SIZE 64000

/**
 * @brief Realiza una query sobre el socket el socket enviado con un intervalo de tiempo sleepTime dado que repeat este en 1 
 * 
 * @param socket Socket al cual se va a conectar
 * @param query Query que se va a correr sobre la DB
 * @param sleepTime Tiempo de "descanso" despues de enviar la query
 * @param repeat 1: Repetir la query de forma continua - 0: No repetir
 */
void sendQuery(int socket, char *query, unsigned int sleepTime, int repeat);

#endif
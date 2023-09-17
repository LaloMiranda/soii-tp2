#include "../inc/clients.h"

void sendQuery(int socket, char *query, unsigned int sleepTime, int repeat){
    char buffer[BUF_SIZE];
    ssize_t res;

    printf("Query a enviar: %s\n", query);
    do{
        // Limpio el buffer
        memset(buffer, 0, BUF_SIZE);

        // Envio la query al servidor
        res = send(socket, query, strlen(query), 0);
        if (res < 0){
            perror("Error al mandar mensaje\n");
        }
            
        // Espero la respuesta del server y guardo la info en buffer
        res = read(socket, buffer, BUF_SIZE);
        if (res < 0){
            perror("Error al recibir la respuesta del servidor\n");
        }

        printf("Respuesta del servidor: %s\n", buffer);
        sleep(sleepTime);
    } while(repeat);
}
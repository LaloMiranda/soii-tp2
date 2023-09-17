#include "../inc/clients.h"

int main(int argc, char const *argv[]){
    if (argc < 2){
		printf("Formato de enrtada: %s <socket> <nombre_de_descarga>\n", argv[0]);
		exit(0);
	}

    // Configuro la conexión
    ssize_t servlen;
	struct sockaddr_un serv_addr;
	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX; // Configuración Unix
	strcpy(serv_addr.sun_path, argv[1]);
	servlen = (long int)strlen(serv_addr.sun_path);
	servlen = servlen + (ssize_t)sizeof(serv_addr.sun_family);

    // Creo el socket de conexión
    int sockfd;
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("Error al crear el socket UNIX");
		exit(1);
	}
	printf("Path del socket: %s\n", serv_addr.sun_path);

    // Establezco conexión
	if (connect(sockfd, (struct sockaddr *)&serv_addr, (socklen_t)servlen) < 0){
		perror("Error al establecer la conexión");
		exit(1);
	}

    // Consigo el path donde voy a guardar la copia de la DB
    char path[100];
	snprintf(path, sizeof(path), "./bin/%s.db", argv[2]);
	printf("Direccion donde se va a guardar el archivo: %s\n", path);

    FILE *fp;
	fp = fopen(path, "ab");
	if (fp == NULL){
		perror("Error al abir el archivo donde se guardara la DB\n");
		exit(1);
	}

	// Traigo la info de la DB en baches de 256 bytes
	int bytesReceived = 0;
	char recvBuff[256];
	while ((bytesReceived = (int)read(sockfd, recvBuff, 256)) > 0){
		fwrite(recvBuff, 1, (size_t)bytesReceived, fp);
	}
	close(sockfd);
	return 0;
}

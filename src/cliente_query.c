#include "../inc/clients.h"

int main(int argc, char const *argv[]){
    if (argc != 3)	{
        printf("Formato de enrtada: %s <server-ip> <portIPv4>\n", argv[0]);
        exit(EXIT_FAILURE);
	}

    // Obtengo puerto de conexión IPv4 como int
	uint16_t puertoIPv4;
    puertoIPv4 = (uint16_t)atoi(argv[2]);
    printf("El puerto es: %d\n", puertoIPv4);

    // Abro el socket IPv4
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		printf("Error al crear el socket IPv4 cliente_continuo\n");
		exit(EXIT_FAILURE);
	}
	printf("Socket fd = %d\n", sockfd);


	// Estructuras para la conexión del socket
	struct sockaddr_in serv_addr;
	struct hostent *server;
    
    // Obtengo valores para la estructura hostent
	server = gethostbyname(argv[1]);
    if(server == NULL){
        perror("Error al obtener la información del host");
        exit(EXIT_FAILURE);
    }

    // Configuro el tipo de conexión
	memset((char *)&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; //IPv4

	bcopy((char *)server->h_addr,
		  (char *)&serv_addr.sin_addr.s_addr,
		  (size_t)server->h_length);

	serv_addr.sin_port = htons(puertoIPv4);
	
    // Intento Conexion
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Cliente continuo: Error de conexión");
		exit(EXIT_FAILURE);
	}
	printf("Conexion Exitosa\n");

    char query[BUF_SIZE];
	while (1){
        // Vacio el buffer
		memset(query, 0, sizeof(query));
        // Obtengo la Query
		printf("Query a realizar: ");
		fgets(query, sizeof(query), stdin);

        //Quito el \n del final
		query[strlen(query) - 1] = '\0';
		if(strcmp(query, "exit") == 0){
			break;
		}

        // Mando la query e imprimo la rta
		sendQuery(sockfd, query, 0, 0);
	}

	close(sockfd);
	return 0;
}
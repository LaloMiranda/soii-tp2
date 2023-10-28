#include "../inc/dependencias.h"
#include "../inc/baseDatos.h"

// FUNCIONES PARA EL CIERRE DE LA APP

/**
 * @brief Arreglo de elementos a cerrar en cerrarServer()
 * 
 * @param val 
 * @param ind 
 */
void regIntGlob(int val, int ind){
	valGlob[ind] = val;
}

/**
 * @brief Funcion que se dispara con SIGKILL en CLI parar cerrar recursos
 * 
 */
void cerrarServer(){
	kill(valGlob[0], SIGTERM);
	kill(valGlob[1], SIGTERM);
	kill(valGlob[2], SIGTERM);
	borrarSemaforo(valGlob[4]);
	borrarMemcomp(valGlob[3]);
	exit(EXIT_SUCCESS);
}

/**
 * @brief Borro memoria compartida
 * 
 * @param shmid 
 */
void borrarMemcomp(int shmid){
	if (shmctl(shmid, IPC_RMID, NULL) < 0){
		perror("[Server] Error Borrar ShMEM\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "[SERVER] Memoria comp borrada\n");
}

// FUNCIONES PARA CREACION Y GESTION DE SEMAFORO

/**
 * @brief Configuracion del semaforo
 * 
 * @param semid 
 * @param num 
 */
void configSemaforo(int semid, int num){
	union semun u;
	u.val = num;
	if (semctl(semid, 0, SETVAL, u) < 0){
		perror("[Server] Error al configurar el semafoto\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "[SERVER] Semaforo configurado\n");
}

/**
 * @brief Funcion con la que creamos el semaforo
 * 
 * @return int 
 */
int crearSemaforo(){
	key_t key = ftok("./sem", 'S');
	int semid;
	if ((semid = semget(key, 1, IPC_CREAT | 0660)) == -1){
		perror("[Server] Error al crear el semafoto\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "[SERVER] Semaforo creado\n");
	return semid;
}

/**
 * @brief borrar el semaforooo
 * 
 * @param semid 
 */

void borrarSemaforo(int semid){
	if (semctl(semid, 0, IPC_RMID, 0) < 0)
	{
		perror("[Server] Error al crear el semafoto\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "\n[SERVER] Semaforo destruido\n");
}

/**
 * @brief Decrementar el semaforo 1 Wait
 * 
 * @param semid 
 */
void decreSem(int semid){
	struct sembuf p = {0, -1, SEM_UNDO}; // semwait
	if (semop(semid, &p, 1) < 0){
		perror("[SERVER-IPv4] Error al decrementar el Semaforo\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Incrementar semaforo 1 SIGNAL
 * 
 * @param semid 
 */
void increSem(int semid){
	struct sembuf v = {0, +1, SEM_UNDO}; // semsignal
	if (semop(semid, &v, 1) < 0){
		perror("[SERVER-IPv4] Error al decrementar el Semaforo\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Funcion que se encarga de la configuracion del socket y la asocia a una configuracion de red IPv4
 * retorna el socket (int)
 * @param puerto 
 * @return int 
 */
int configSockerIPv4(uint16_t puerto){
	int serverSocketIPv4;
	struct sockaddr_in servAddr;
	serverSocketIPv4 = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocketIPv4 < 0){
		perror("Error al crear el socket IPv4\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-IPv4]: Socket creado!\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;          // Configuracion IPv4
	servAddr.sin_addr.s_addr = INADDR_ANY;	// Cualquier ip de la PC
	servAddr.sin_port = htons(puerto);		  // Puerto de la conexion

	if (bind(serverSocketIPv4, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
		perror("[SERVER-IPv4-ERROR] ERROR: No fue posible asignar el socket\n");
		close(serverSocketIPv4);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-IPv4]: Socket asociado!\n");

	if (listen(serverSocketIPv4, MAX_CLIENT) < 0){
		perror("[SERVER-IPv4-ERROR] Error al poner el socket del sv en listen\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-IPv4]: Socket configurado!\n");
	return serverSocketIPv4;
}

/**
 * @brief Funcion que se encarga de la configuracion del socket y la asocia a una configuracion de red IPv6
 * retorna el socket (int)
 * @param puerto 
 * @return int 
 */

int configSockerIPv6(uint16_t puerto){
	int serverSocketIPv6;
	struct sockaddr_in6 servAddr;

	serverSocketIPv6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocketIPv6 == -1){
		perror("Error al crear el socket IPv6");
		return EXIT_FAILURE;
	}
	fprintf(stdout, "[SERVER-IPv6]: Socket creado!\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin6_family = AF_INET6;      // Configuracion IPv6
	servAddr.sin6_addr = in6addr_any;     // Cualquier ip de la PC
	servAddr.sin6_port = htons(puerto);   // Puerto de la conexion

	if (bind(serverSocketIPv6, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
		perror("[SERVER-IPv6-ERROR] ERROR: No fue posible asignar el socket\n");
		close(serverSocketIPv6);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-IPv6]: Socket asociado!\n");

	if (listen(serverSocketIPv6, MAX_CLIENT) < 0){
		perror("[SERVER-IPv6-ERROR] Error al poner el socket del sv en listen\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-IPv6]: Socket configurado!\n");

	return serverSocketIPv6;
}

/**
 * @brief Funcion que se encarga de la configuracion del socket y la asocia a una configuracion de UNIX
 * Y recibe conexiones generando un hijo por cada conexion
 * @param puerto 
 * @return int 
 */

void configSocketUnix(char *path, BasesDatos *db, int semid){
	int sockfd, newsockfd, servlen;
	struct sockaddr_un cli_addr, serv_addr;
	char buffer[BUF_SIZE];
	socklen_t client_len;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("Error al crear socket UNIX");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-UNIX]: Socket creado!\n");

	unlink(path);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;     // Configuracion UNIX
	strcpy(serv_addr.sun_path, path);
	servlen = (int)strlen(serv_addr.sun_path) + (int)sizeof(serv_addr.sun_family);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, (socklen_t)servlen) < 0){
		perror("ligadura");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "[SERVER-UNIX]: Socket asociado!\n");

	if (listen(sockfd, MAX_CLIENT) < 0){
		perror("[SERVER-UNIX-ERROR] Error al poner el socket en listen\n");
		exit(EXIT_FAILURE);
	}
	else
	fprintf(stdout, "[SERVER-UNIX]: Socket configurado!\n");

	while (1){
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &client_len);
		if (newsockfd == -1){
			fprintf(stdout, "[PID: %d] [SERVER-UNIX-ERROR] Fallo al aceptar el cliente.\n", getpid());
			exit(EXIT_FAILURE);
		}

		int ch_pid = fork();
		if (ch_pid == -1){
			fprintf(stdout, "[PID: %d] [SERVER-UNIX-ERROR] fallo al hacer el fork.\n", getpid());

			exit(EXIT_FAILURE);
		}

		if (ch_pid == 0){
			close(sockfd);
			getQueryBaseDatos("Obtuvo una copia de la base de datos", db, 3, semid);
			while (1){
				memset(buffer, '\0', BUF_SIZE);
				sendBaseDatos(newsockfd);
			}
		}
		else{
			// Proceso padre
			fprintf(stdout, "[PID: %d] [SERVER-UNISX] Nuevo cliente PID: %d.\n", getppid(), ch_pid);
			close(newsockfd);
		}
	}
}

/**
 * @brief Ahora dado un puerto y el lugar a donde guardar los datos, se crea el socket y acepta clientes para tomar sus datos y almacenarlos 
 * en DATOS
 * 
 * @param puerto 
 * @param datos 
 */
void serverIPv4forks(uint16_t puerto, BasesDatos *db, int semid){
	int serverSocketIPv4;
	char buff_rx[BUF_SIZE]; /* buffer for reception  */ /* Para la direccion del client */
	struct sockaddr_in clientAddr;
	serverSocketIPv4 = configSockerIPv4(puerto);

	socklen_t client_len;
	client_len = sizeof(clientAddr);

	while (1){
		int cl_socket_fd = accept(serverSocketIPv4, (struct sockaddr *)&clientAddr, &client_len);

		if (cl_socket_fd == -1){
			fprintf(stdout, "[PID: %d] [SERVER-IPv4-ERROR] Fallo al aceptar el cliente.\n", getpid());
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "[PID: %d] [SERVER-IPv4] Acepto al cliente.\n", getpid());

		int ch_pid = fork();
		if (ch_pid == -1){
			fprintf(stdout, "[PID: %d] [SERVER-IPv4-ERROR] fallo al hacer el fork.\n", getpid());
			exit(EXIT_FAILURE);
		}

    // Proceso hijo dps del fork
		if (ch_pid == 0)
		{
			close(serverSocketIPv4);
			while (1){
				char *result;
				memset(buff_rx, 0, BUF_SIZE);
				long rec = read(cl_socket_fd, buff_rx, (BUF_SIZE - 1));
				if (rec == -1){
					fprintf(stdout, "[PID: %d] [SERVER-IPv4-ERROR] Fallo al recibir el mensaje.\n", getppid());
					exit(EXIT_FAILURE);
				}

				if (rec == 0){
					fprintf(stdout, "[PID: %d] [SERVER-IPv4] Cliente con PID: %d se desconecto.\n", getppid(), getpid());
					exit(EXIT_FAILURE);
				}

				result = getQueryBaseDatos(buff_rx, db, 1, semid);
				if (send(cl_socket_fd, result, strlen(result), 0) < 0){
					perror("Error al mandar mensaje\n");
					exit(EXIT_FAILURE);
				}
				memset(result, 0, BUF_SIZE);
			}
		}
		else{
			// Proceso padre
			fprintf(stdout, "[PID: %d] [SERVER-IPv4] Nuevo cliente PID: %d.\n", getpid(), ch_pid);
			close(cl_socket_fd);
		}
	}
}

/** 
 * @brief Ahora dado un puerto y el lugar a donde guardar los datos, se crea el socket y acepta clientes para tomar sus datos y almacenarlos 
 * en DATOS
 * 
 * @param puerto 
 * @param datos 
 */
void serverIPv6forks(uint16_t puerto, BasesDatos *db, int semid){
	int serverSocketIPv6;
	char buff_rx[BUF_SIZE];
	struct sockaddr_in clientAddr;
	serverSocketIPv6 = configSockerIPv6(puerto);
	socklen_t client_len;
	client_len = sizeof(clientAddr);

	while (1){
		int cl_socket_fd = accept(serverSocketIPv6, (struct sockaddr *)&clientAddr, &client_len);
		if (cl_socket_fd == -1){
			fprintf(stdout, "[PID: %d] [SERVER-IPv6-ERROR] Fallo al aceptar el cliente.\n", getpid());
			exit(EXIT_FAILURE);
		}

		int ch_pid = fork();
		if (ch_pid == -1){
			fprintf(stdout, "[PID: %d] [SERVER-IPv6-ERROR] Fallor al crear el fork.\n", getpid());

			exit(EXIT_FAILURE);
		}

    // Proceso hijo dps del fork
		if (ch_pid == 0){
			close(serverSocketIPv6);
			while (1){
				char *result;
				memset(buff_rx, 0, BUF_SIZE);
				long rec = read(cl_socket_fd, buff_rx, (BUF_SIZE - 1));
				if (rec == -1){
					fprintf(stdout, "[PID: %d] [SERVER-IPv6-ERROR] Fallo al recibir el mensaje.\n", getpid());
					exit(EXIT_FAILURE);
				}

				if (rec == 0){
					fprintf(stdout, "[PID: %d] [SERVER-IPv6] Cliente con PID: %d se desconecto.\n", getppid(), getpid());
					exit(EXIT_FAILURE);
				}

				result = getQueryBaseDatos(buff_rx, db, 2, semid);
				if (strlen(result) == 0){
					//printf("MANDE ESTO PORQUE ES CERO\n");
					if (send(cl_socket_fd, " ", 1, 0) < 0)
						perror("Error al mandar mensaje\n");
				}
				else{
					if (send(cl_socket_fd, result, strlen(result), 0) < 0){
            perror("Error al mandar mensaje\n");
          }
				}
				bzero(result, strlen(result));
			}
		}
		else{
			// Proceso padre
			fprintf(stdout, "[PID: %d] [SERVER-IPv6] Nuevo cliente PID: %d.\n", getpid(), ch_pid);
			close(cl_socket_fd);
		}
	}
}

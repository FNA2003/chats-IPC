#include <fcntl.h>  // Para manejo de file-descriptors
#include <stdio.h>  // Manejo de I/O
#include <stdlib.h> // EXIT_FAILURE, atoi() entre otros
#include <unistd.h> // fork(), pid_t entre otros
#include <wait.h>   // waitpid()
#include <string.h> // strncmp(), strlen(), memset()
#include <signal.h> // Usamos la señal SIGCHLD para que el hijo lector le avise al padre cuando terminó de leer (otro extremo envió protocolo de salida)


#define PROTOCOLO_SALIDA "bye"
#define BUFFER_SIZE 256

static void procesoLector(const int fdLectura);
static void procesoEscritor(const int fdEscritura);

/* Variable gloabal para que use la señal cuando termine el proceso hijo para finalizar al padre */
sig_atomic_t seguirEscribiendo = 1;
void senialPadre(int senialN);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("ERROR DE ARGUMENTOS!\n\tUso: './chatfifo <fifo1> <fifo2>'\n");
		return EXIT_FAILURE;
	}
	int fifoLectura, fifoEscritura;
	pid_t pidLector;
	
	/* Voy a abrir los fifo's de una manera secuencial para tratar de evitar el bloqueo ante 
	la espera de uno u otro (no hay sin. de procesos). Además, ante cualquier error cierro todo y fin de programa */
	// 1. Abro el fifo de lectura en modo no-bloqueante
	fifoLectura = open(argv[2], O_RDONLY | O_NONBLOCK);
	if (fifoLectura < 0) {
		perror("Error al tratar de abrir el fifo de lectura '<fifo2>'");
		return EXIT_FAILURE;
	}
	// 2. Abro el fifo correspondiente a la escritura con el fifo lector YA ABIERTO
	fifoEscritura = open(argv[1], O_WRONLY);
	if (fifoEscritura < 0) {
		perror("Error al tratar de abrir el fifo de escritura '<fifo1>'");
		close(fifoLectura);
		return EXIT_FAILURE;
	}
	// 3. Cerramos el fifo lector no-bloqueante y lo abrimos de manera bloqueante (normal)
	close(fifoLectura);
	fifoLectura = open(argv[2], O_RDONLY);
	if (fifoLectura < 0) {
		perror("Error al tratar de abrir el fifo de escritura '<fifo1>'");
		close(fifoEscritura);
		return EXIT_FAILURE;
	}
	
	/* Ahora, trato de crear el proceso lector y, en caso de poder
	asigno las funciones a cada proceso */
	pidLector = fork();
	if (pidLector < 0) { // Caso de error
		perror("Error al crear el proceso lector");
		return EXIT_FAILURE;
	} else if (pidLector == 0) { // Proceso hijo, será el lector
		close(fifoEscritura);
		procesoLector(fifoLectura);
		close(fifoLectura);
		return EXIT_SUCCESS;
	} else { // Proceso padre, será el proceso escritor
		signal(SIGCHLD, senialPadre); // Señal para, que si termina antes el proceso hijo, cortemos al padre
		close(fifoLectura);
		procesoEscritor(fifoEscritura);
		close(fifoEscritura);
	}
	
	kill(pidLector, SIGKILL); // Y, esta señal, es por si termina el padre antes que el hijo
	waitpid(pidLector, NULL, 0);
	return EXIT_SUCCESS;
}

static void procesoLector(const int fdLectura) {
	char bufferIn[BUFFER_SIZE + 1];
	const size_t PROTOCOLO_SIZE = strlen(PROTOCOLO_SALIDA);
	int caracteresLeidos = 0;
	
	do {
		memset(bufferIn, '\0', BUFFER_SIZE + 1);

		caracteresLeidos = read(fdLectura, bufferIn, BUFFER_SIZE);
		if (caracteresLeidos < 0) {
			perror("Error en la lectura del FIFO");
			return;
		} else if (caracteresLeidos == 0) { // EOF, el escritor cerró su extremo
			break;
		}
		bufferIn[caracteresLeidos-1] = '\0'; // Borro el '\n'
		printf("[Usuario]: %s\n", bufferIn);
	} while(strncmp(bufferIn, PROTOCOLO_SALIDA, PROTOCOLO_SIZE) != 0);
}
static void procesoEscritor(const int fdEscritura) {
	char bufferOut[BUFFER_SIZE + 1];
	const size_t PROTOCOLO_SIZE = strlen(PROTOCOLO_SALIDA);
	size_t tamanioMensaje;
	
	do {
		memset(bufferOut, '\0', BUFFER_SIZE + 1);

		printf("> ");
		// fgets() siempre agrega un '\0'
		if (fgets(bufferOut, BUFFER_SIZE+1, stdin) == NULL) {
			perror("Error al leer entrada estándar");
			return;
		}
		tamanioMensaje = strlen(bufferOut); // Evito mandar basura al comprobar la cantidad de caracteres de entrada
		if (write(fdEscritura, bufferOut, tamanioMensaje) < 0) {
			perror("Error al escribir en el FIFO");
			return;
		}
	} while(strncmp(bufferOut, PROTOCOLO_SALIDA, PROTOCOLO_SIZE) != 0 && seguirEscribiendo);
}

void senialPadre(int senialN) {
	printf("[DEBUG] El proceso hijo recibió el protocolo de corte, cerrando padre en la próxima iteración!\n");
	seguirEscribiendo = 0;	
}

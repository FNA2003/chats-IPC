#include <fcntl.h>  // Para manejo de file-descriptors
#include <stdio.h>  // Manejo de I/O
#include <stdlib.h> // EXIT_FAILURE, atoi() entre otros
#include <unistd.h> // fork(), pid_t entre otros
#include <wait.h>   // waitpid()
#include <string.h> // strncmp(), strlen()


#define PROTOCOLO_SALIDA "bye"
#define BUFFER_SIZE 256

static void procesoLector(const int fdLectura);
static void procesoEscritor(const int fdEscritura);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("ERROR DE ARGUMENTOS!\n\tUso: './chatfifo <fifo1> <fifo2>'\n");
		return EXIT_FAILURE;
	}
	int fifoLectura, fifoEscritura;
	pid_t pidLector;
	
	/* Abro los fifo's si se puede, sino, retorno */
	fifoEscritura = open(argv[1], O_WRONLY, O_NONBLOCK); // El de escritura por defecto lo abrimos no bloqueante para que pueda llegar al de lectura
	if (fifoEscritura < 0) {
		perror("Error al tratar de abrir el fifo de escritura '<fifo1>'");
		return EXIT_FAILURE;
	}
	
	fifoLectura = open(argv[2], O_RDONLY);
	if (fifoLectura < 0) {
		perror("Error al tratar de abrir el fifo de lectura '<fifo2>'");
		close(fifoEscritura);
		return EXIT_FAILURE;
	}
	
	/* Ahora, trato de crear el proceso lector y, en caso de poder
	asigno las funciones de cada proceso */
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
		close(fifoLectura);
		procesoEscritor(fifoEscritura);
		close(fifoEscritura);
	}
	
	waitpid(pidLector, NULL, 0);
	return EXIT_SUCCESS;
}

static void procesoLector(const int fdLectura) {
	char bufferIn[BUFFER_SIZE + 1];
	const size_t PROTOCOLO_SIZE = strlen(PROTOCOLO_SALIDA);
	int caracteresLeidos = 0;
	
	do {
		caracteresLeidos = read(fdLectura, bufferIn, BUFFER_SIZE);
		if (caracteresLeidos < 0) {
			perror("Error en la lectura del FIFO");
			return;
		}
		bufferIn[caracteresLeidos] = '\0';
		printf("[Usuario]: %s\n", bufferIn);
	} while(strncmp(bufferIn, PROTOCOLO_SALIDA, PROTOCOLO_SIZE) != 0 || caracteresLeidos > 0);
}
static void procesoEscritor(const int fdEscritura) {
	char bufferOut[BUFFER_SIZE + 1];
	const size_t PROTOCOLO_SIZE = strlen(PROTOCOLO_SALIDA);
	
	do {
		printf("> ");
		// fgets() siempre agrega un '\0'
		if (fgets(bufferOut, BUFFER_SIZE+1, stdin) == NULL) {
			perror("Error al leer entrada estándar");
			return;
		}
		if (write(fdEscritura, bufferOut, BUFFER_SIZE+1) < 0) {
			perror("Error al escribir en el FIFO");
			return;
		}
	} while(strncmp(bufferOut, PROTOCOLO_SALIDA, PROTOCOLO_SIZE) != 0);
}

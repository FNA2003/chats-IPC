#include <fcntl.h>  // Para manejo de file descriptors
#include <stdio.h>  // Entrada/Salida estándar
#include <stdlib.h> // EXIT_FAILURE, atoi(), etc.
#include <unistd.h> // fork(), pid_t, read(), write(), close()
#include <wait.h>   // waitpid()
#include <string.h> // strncmp(), strlen(), memset()
#include <signal.h> // signal(), SIGCHLD, SIGTERM, kill()

#define PROTOCOLO_SALIDA "bye"
#define BUFFER_SIZE 256

// Variable global modificada por la señal SIGCHLD
sig_atomic_t seguirEscribiendo = 1;

// Prototipos de funciones
static void ejecutarLector(const int fdLectura);
static void ejecutarEscritor(const int fdEscritura);
void manejarSenialHijo(int senial);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso incorrecto.\n\tEjemplo: ./chatfifo <fifo_escritura> <fifo_lectura>\n");
        return EXIT_FAILURE;
    }


    const char* nombreFifoEscritura = argv[1];
    const char* nombreFifoLectura   = argv[2];

    int fdLectura, fdEscritura;
    pid_t pidLector;

    // Paso 1: Abrir FIFO de lectura en modo no bloqueante
    fdLectura = open(nombreFifoLectura, O_RDONLY | O_NONBLOCK);
    if (fdLectura < 0) {
        perror("Error al abrir FIFO de lectura (no bloqueante)");
        return EXIT_FAILURE;
    }

    // Paso 2: Abrir FIFO de escritura
    fdEscritura = open(nombreFifoEscritura, O_WRONLY);
    if (fdEscritura < 0) {
        perror("Error al abrir FIFO de escritura");
        close(fdLectura);
        return EXIT_FAILURE;
    }

    // Paso 3: Reabrir FIFO de lectura en modo bloqueante
    close(fdLectura);
    fdLectura = open(nombreFifoLectura, O_RDONLY);
    if (fdLectura < 0) {
        perror("Error al reabrir FIFO de lectura (bloqueante)");
        close(fdEscritura);
        return EXIT_FAILURE;
    }
    
	// Crear proceso hijo (lector)
    pidLector = fork();
    if (pidLector < 0) {
        perror("Error al crear proceso hijo");
        return EXIT_FAILURE;
    }

    if (pidLector == 0) {
        // Proceso hijo (será el lector)
        close(fdEscritura);
        ejecutarLector(fdLectura);
        close(fdLectura);
        return EXIT_SUCCESS;
    } else {
        // Proceso padre (escritor del pipe)
        signal(SIGCHLD, manejarSenialHijo); // "Detectar" el fin del hijo
        close(fdLectura);
        ejecutarEscritor(fdEscritura);
        close(fdEscritura);
    }

    // Finalizar hijo si aún está activo
    kill(pidLector, SIGTERM);
    waitpid(pidLector, NULL, 0);

    return EXIT_SUCCESS;
}

static void ejecutarLector(const int fdLectura) {
	char buffer[BUFFER_SIZE + 1];
	const size_t largoProtocolo = strlen(PROTOCOLO_SALIDA);
	int leidos;
	
	do {
		memset(buffer, '\0', BUFFER_SIZE + 1);

		leidos = read(fdLectura, buffer, BUFFER_SIZE);
		if (leidos < 0) {
			perror("Error al leer del FIFO");
			return;
		} else if (leidos == 0) {
			// El otro extremo cerró el FIFO
			break;
		}
		
		buffer[leidos - 1] = '\0'; // Borro el '\n'
		printf("[Usuario]: %s\n", buffer);
		
	} while(strncmp(buffer, PROTOCOLO_SALIDA, largoProtocolo) != 0);
}
static void ejecutarEscritor(const int fdEscritura) {
	char buffer[BUFFER_SIZE + 1];
	const size_t largoProtocolo = strlen(PROTOCOLO_SALIDA);
	size_t largoMensaje;
	
	do {
		memset(buffer, '\0', BUFFER_SIZE + 1);

		printf("> ");
		if (fgets(buffer, BUFFER_SIZE + 1, stdin) == NULL) {
			perror("Error al leer la entrada estándar");
			return;
		}
		
		// Evito mandar basura al comprobar la cantidad de caracteres de entrada
		largoMensaje = strlen(buffer);
		if (write(fdEscritura, buffer, largoMensaje) < 0) {
			perror("Error al escribir en el FIFO");
			return;
		}
	} while(strncmp(buffer, PROTOCOLO_SALIDA, largoProtocolo) != 0 && seguirEscribiendo);
}

void manejarSenialHijo(int senial) {
	seguirEscribiendo = 0;	
}

#include <sys/msg.h> // Librería de cola de mensajes
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

extern int errno;

#define BUFFER_SIZE 256
#define COLA_ADDR 0xA
#define PROTOCOLO_SALIDA "bye"

sig_atomic_t ejecutarEscritor = 1;

typedef struct msgbuf {
	long type; 				 // Tipo de mensaje, requerido por sys/msg.h
	char mtext[BUFFER_SIZE]; // Mensaje a enviar, en nuestro caso, un arreglo de char's
} msgbuf;

static void leerCola(int cola_id, msgbuf colaLectura);
static void escribirCola(int cola_id, msgbuf colaEscritura);
void finalizarPadre(int senial);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso incorrecto\n\tEjecutar cómo: ./chatColas <tipo_cola_lectura> <tipo_cola_escritura>\n");
		return EXIT_FAILURE;
	}
	long tipoLectura = atol(argv[1]);
	long tipoEscritura = atol(argv[2]);
	
	msgbuf colaEscritura;
	msgbuf colaLectura;
		
	int cola_id;
	pid_t procesoLector;
	
	
	signal(SIGCHLD, finalizarPadre); // Señal que usará el padre para finalizar la escritura si el hijo recibió un "bye" y se fué
	
	// Empiezo por verificar si hubo un error de parámetros
	if (tipoLectura == tipoEscritura) {
		fprintf(stderr, "Los tipos de colas de mensajes ingresados son iguales ó existió un error al castear\n");
		return EXIT_FAILURE;
	}

	// Luego, tratamos de abrir, o crear la cola de mensajes (Abrir -> Somos el segundo proceso, Crear -> Primero)
	cola_id = msgget(COLA_ADDR, 0);
	if (cola_id < 0 && errno == ENOENT)
		cola_id = msgget(COLA_ADDR, IPC_CREAT | IPC_EXCL | 0600);
	if (cola_id < 0) {
		perror("Error al abrir/crear la cola de mensajes");
		return EXIT_FAILURE;
	}
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mID de la cola de mensajes: %d\033[0m\n", cola_id);
	
	
	// Asigno a la estructura de la cola de mensajes el tipo de recepción y emisión
	colaEscritura.type = tipoEscritura;
	colaLectura.type = tipoLectura;
	
	// Creo el proceso lector y le asigno a cada uno su función
	procesoLector = fork();
	if (procesoLector < 0) {
		perror("Error al crear el proceso lector"); // Nota; no borro la cola de mensajes para que se vuelva a ejecutar el programa
		return EXIT_FAILURE;
	}
	
	if (procesoLector == 0) {
		leerCola(cola_id, colaLectura);
		return EXIT_SUCCESS;
	} else {
		escribirCola(cola_id, colaEscritura);
	}	
	
	kill(SIGTERM, procesoLector); // Forzamos al hijo a que finalize, si es que finalizó el padre
	waitpid(procesoLector, NULL, 0);
	
	// Nota: Como los dos procesos, al terminar ejecutarán esta línea, uno fallará al tratar de borrar la cola de mensajes
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mTratando de borrar la cola de mensajes...\033[0m\n");
	msgctl(cola_id, IPC_RMID, NULL);
	
	return EXIT_SUCCESS;
}

static void leerCola(int cola_id, msgbuf colaLectura) {
	const size_t tamanioProtocolo = strlen(PROTOCOLO_SALIDA);
	
	do {
		memset(colaLectura.mtext, (int) '\0', BUFFER_SIZE);
		
		// Recibo el mensaje a la cola especificada de lectura, y, verifico si hubo un error
		if (msgrcv(cola_id, (void *) &colaLectura, sizeof(msgbuf), colaLectura.type, 0) < 0) {
			perror("Error al leer la cola de mensajes");
			return;
		}
		
		printf("[USUARIO] %s\n", colaLectura.mtext);
		
	} while(strncmp(colaLectura.mtext, PROTOCOLO_SALIDA, tamanioProtocolo));
}
static void escribirCola(int cola_id, msgbuf colaEscritura) {
	const size_t tamanioProtocolo = strlen(PROTOCOLO_SALIDA);
	
	do {
		memset(colaEscritura.mtext, (int) '\0', BUFFER_SIZE);
		printf(">: ");

		// NOTA: fgets() me asegura el \0, por eso no debo agregarlo o restarle longitud al buffer
		if (fgets(colaEscritura.mtext, BUFFER_SIZE, stdin) == NULL) {
			perror("Error al leer la entrada estándar");
			return;
		}
		
		msgsnd(cola_id, (void *)&colaEscritura, sizeof(msgbuf), 0);
	} while(strncmp(colaEscritura.mtext, PROTOCOLO_SALIDA, tamanioProtocolo) && ejecutarEscritor);
}

void finalizarPadre(int senial) {
	ejecutarEscritor = 0;
}

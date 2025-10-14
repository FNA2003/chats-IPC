#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>

#define BUFFER_SIZE 256
#define COLA_ADDR 0xA
#define PROTOCOLO_SALIDA "bye"

typedef struct msgbuf {
	long type;
	char mtext[BUFFER_SIZE];
} msgbuf;

sig_atomic_t seguirEscribiendo = 1;

// Prototipos
static void leerCola(int cola_id, int tipoLectura);
static void escribirCola(int cola_id, int tipoEscritura);
void finalizarPadre(int senial);


int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso:\n\t./chatColas <tipo_cola_lectura> <tipo_cola_escritura>\n");
		return EXIT_FAILURE;
	}
	
	long tipoLectura = atol(argv[1]);
	long tipoEscritura = atol(argv[2]);
	// Empiezo por verificar si hubo un error de parámetros
	if (tipoLectura == tipoEscritura) {
		fprintf(stderr, "Los tipos de colas de mensajes ingresados son iguales ó existió un error al castear\n");
		return EXIT_FAILURE;
	}
	
	// Señal que usará el padre para finalizar la escritura si el hijo recibió un "bye" y se fué
	signal(SIGCHLD, finalizarPadre);

	// Luego, tratamos de abrir, o crear la cola de mensajes (Abrir -> Somos el segundo proceso, Crear -> Primero)
	int cola_id = msgget(COLA_ADDR, 0);
	if (cola_id < 0 && errno == ENOENT)
		cola_id = msgget(COLA_ADDR, IPC_CREAT | IPC_EXCL | 0600);
	if (cola_id < 0) {
		perror("Error al abrir/crear la cola de mensajes");
		return EXIT_FAILURE;
	}
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mID de la cola de mensajes: %d\033[0m\n", cola_id);

	
	// Creo el proceso lector y le asigno a cada uno su función
	pid_t procesoLector = fork();
	if (procesoLector < 0) {
		perror("Error al crear el proceso lector"); // Nota; no borro la cola de mensajes para que se vuelva a ejecutar el programa
		return EXIT_FAILURE;
	}
	
	if (procesoLector == 0) {
		leerCola(cola_id, tipoLectura);
		return EXIT_SUCCESS;
	} else {
		escribirCola(cola_id, tipoEscritura);
	}	
	
	kill(procesoLector, SIGTERM); // Forzamos al hijo a que finalize, si es que finalizó antes el padre
	waitpid(procesoLector, NULL, 0);
	
	// Nota: Como los dos procesos, al terminar ejecutarán esta línea, uno fallará al tratar de borrar la cola de mensajes
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mTratando de borrar la cola de mensajes...\033[0m\n");
	msgctl(cola_id, IPC_RMID, NULL);
	
	return EXIT_SUCCESS;
}

static void leerCola(int cola_id, int tipoLectura) {
	msgbuf mensaje;
	mensaje.type = tipoLectura;
	
	do {
		memset(mensaje.mtext, '\0', BUFFER_SIZE);
		
		// Leo de la cola de mensajes y verifico errores
		ssize_t bytes = msgrcv(cola_id, (void *) &mensaje, sizeof(msgbuf), tipoLectura, 0);
		if (bytes < 0){
			perror("Error al leer mensaje");
			return;
		}
		mensaje.mtext[bytes] = '\0'; // Apesar que el que escriba asegura el caracter nulo, lo aseguro
		printf("[USUARIO] %s\n", mensaje.mtext);
		
	} while(strncmp(mensaje.mtext, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}

static void escribirCola(int cola_id, int tipoEscritura) {
	msgbuf mensaje;
	mensaje.type = tipoEscritura;
	
	do {
		memset(mensaje.mtext, (int) '\0', BUFFER_SIZE);
		printf(">: ");

		// fgets() asegura el nulo
		if (fgets(mensaje.mtext, BUFFER_SIZE, stdin) == NULL) {
			perror("Error al leer la entrada estándar");
			return;
		}
		
        mensaje.mtext[strcspn(mensaje.mtext, "\n")] = '\0'; // Removemos el salto de línea
		
		if (msgsnd(cola_id, (void *)&mensaje, sizeof(msgbuf), 0) < 0) {
			perror("Error al enviar el mensaje");
			return;
		}
	} while(strncmp(mensaje.mtext, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0 && seguirEscribiendo);
}

void finalizarPadre(int senial) {
	seguirEscribiendo = 0;
}

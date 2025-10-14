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
#define COLA_ADDR 0xA 	       // Clave para identificar la cola de mensajes
#define PROTOCOLO_SALIDA "bye" // Palabra para finalizar el chat

// Estructura de mensajes para la cola, la defino como tipo para evitar tener que usar la palabra clave todo el rato
typedef struct msgbuf {
	long type;			     // Tipo de mensaje (requerido por la api)
	char mtext[BUFFER_SIZE]; // Contenido (en nuestro caso un arreglo de caracteres)
} msgbuf;

// Variable global para controlar si el padre debe seguir escribiendo
sig_atomic_t seguirEscribiendo = 1;

// Prototipos de funciones
static void leerCola(int cola_id, int tipoLectura);
static void escribirCola(int cola_id, int tipoEscritura);
void finalizarPadre(int senial);


int main(int argc, char *argv[]) {
	// Validación de argumentos
	if (argc != 3) {
		fprintf(stderr, "Uso:\n\t./chatColas <tipo_cola_lectura> <tipo_cola_escritura>\n");
		return EXIT_FAILURE;
	}
	
	long tipoLectura = atol(argv[1]);
	long tipoEscritura = atol(argv[2]);
	// Verifico que los tipos de mensaje de escritura y lectura sean distintos
	// NOTA: No me interesa si atol() es erroneo, por que en ese caso, retornará un 0, tipo de mensaje aceptado por la cola de mensajes
	if (tipoLectura == tipoEscritura) {
		fprintf(stderr, "Los tipos de colas de mensajes ingresados son iguales ó existió un error al castear\n");
		return EXIT_FAILURE;
	}
	
	// Configuro la señal para detectar cuando el hijo termina
	signal(SIGCHLD, finalizarPadre);

    // Intento abrir la cola de mensajes existente, o crearla si no existe
    int cola_id = msgget(COLA_ADDR, 0);
	if (cola_id < 0 && errno == ENOENT)
		cola_id = msgget(COLA_ADDR, IPC_CREAT | IPC_EXCL | 0600);
	if (cola_id < 0) {
		perror("Error al abrir/crear la cola de mensajes");
		return EXIT_FAILURE;
	}
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mID de la cola de mensajes: %d\033[0m\n", cola_id);

	
	// Creación del proceso hijo (lector)
	pid_t procesoLector = fork();
	if (procesoLector < 0) {
		perror("Error al crear el proceso lector");
		return EXIT_FAILURE;
	}
	
	if (procesoLector == 0) {
		// Proceso hijo: Lee mensajes
		leerCola(cola_id, tipoLectura);
		return EXIT_SUCCESS;
	} else {
		// Proceso padre: Escribe los mensajes que recibe de stdin
		escribirCola(cola_id, tipoEscritura);
	}	
	
	// Finalizo al proceso hijo si todavía está activo
	kill(procesoLector, SIGTERM);
	waitpid(procesoLector, NULL, 0);
	
	// Intento borrar la cola de mensajes (solamente uno de los dos 'usuarios' lo hará)
	printf("\033[1;32m[DEBUG]\033[0m\033\n\t[37;44mTratando de borrar la cola de mensajes...\033[0m\n");
	msgctl(cola_id, IPC_RMID, NULL);
	
	return EXIT_SUCCESS;
}

// Función que lee los mensajes de la queue según el tipo especificado
static void leerCola(int cola_id, int tipoLectura) {
	msgbuf mensaje;
	mensaje.type = tipoLectura;
	
	do {
		memset(mensaje.mtext, '\0', BUFFER_SIZE);
		
		// Lectura bloqueante de la cola
		ssize_t bytes = msgrcv(cola_id, (void *) &mensaje, sizeof(msgbuf), tipoLectura, 0);
		if (bytes < 0){
			perror("Error al leer mensaje");
			return;
		}
		mensaje.mtext[bytes] = '\0'; // Apesar que el escritor agregue el nulo, lo aseguro
		printf("[USUARIO] %s\n", mensaje.mtext);
		
	} while(strncmp(mensaje.mtext, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}

// Función que escribe mensajes en la cola especificada
static void escribirCola(int cola_id, int tipoEscritura) {
	msgbuf mensaje;
	mensaje.type = tipoEscritura;
	
	do {
		memset(mensaje.mtext, '\0', BUFFER_SIZE);
		printf(">: ");

		// Lectura de stdin
		// Nota: fgets() asegura el nulo
		if (fgets(mensaje.mtext, BUFFER_SIZE, stdin) == NULL) {
			perror("Error al leer la entrada estándar");
			return;
		}
		
		// Borro el salto de línea
        mensaje.mtext[strcspn(mensaje.mtext, "\n")] = '\0';
		
		// Envío el mensaje a la cola
		if (msgsnd(cola_id, (void *)&mensaje, sizeof(msgbuf), 0) < 0) {
			perror("Error al enviar el mensaje");
			return;
		}
	} while(strncmp(mensaje.mtext, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0 && seguirEscribiendo);
}

// Manejador de señal que se llama cuando termina el hijo para avisar al padre que debe finalizar
void finalizarPadre(int senial) {
	seguirEscribiendo = 0;
}

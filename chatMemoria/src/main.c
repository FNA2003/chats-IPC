#include "memoria/memoria.h"
#include "semaforos/semaforos.h"
#include "enlace/enlace.h"

#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>


sig_atomic_t terminar = 0; // Variable que usará el consumidor para
void manejoHijo(int);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso:\n\t./chatMemoria <type_t:Decimal del semáforo|memoria de escritura> <type_t:Decimal del semáforo|memoria de escritura>\n");
		return -1;
	}
	int semEscID, semLecID; // Id's de los semáforos
	int shmEscID, shmLecID; // Ahora, id's de la memoria compartida
	key_t kEsc, kLec; 		// Claves para darle a los semaforos y memoria compartida para sincronización

	char *ptrShm; // El proceso escritor la usa para referenciar a la memoria de escritura y vicebersa

	pid_t procesoLector;


	signal(SIGUSR1, manejoHijo); // Señal para que en la librería enlace exista una comunicación de cuando terminó el proceso

	// Trato de obtener la clave de escritura
	if ((kEsc = (key_t) atoi(argv[1])) <= 0) {
		fprintf(stderr, "Clave de recurso de escritura inválida!\n");
		return -2;
	}
	
	// Trata de obtener la clave de lectura
	if ((kLec = (key_t) atoi(argv[2])) <= 0) {
		fprintf(stderr, "Clave de recurso de lectura inválida!\n");
		return -3;
	}

	// Trato de inicializar obtener/crear la memoria compartida
	if (inicializarMemoriasCompartidas(kEsc, kLec, &shmEscID, &shmLecID) < 0)
		return -4; // No muestro mensaje, la función ya lo hace
	
	// Obtengo o creo los semáforos necesarios
	if (getSem(kEsc, kLec, &semEscID, &semLecID) < 0)
		return -5; // Nuevamente, el mensaje de error lo maneja la función


	procesoLector = fork();
	if (procesoLector < 0) {
		perror("Error al crear el proceso lector");
		return -6;
	}

	if (procesoLector == 0) {
		// Proceso hijo, el lector
		ptrShm = (char*) shmat(shmLecID, NULL, 0);
		if (ptrShm == (char*) -1) {
			fprintf(stderr, "Error al asignar el segmento de memoria del lector\n");
			return -1;
		}

		consumidor(semLecID, ptrShm); // Lectura

		shmdt(ptrShm);
		return 0; // Los limpiadores de IPC solamente los ejecuta el padre
	} else {
		// Proceso padre, este escribirá
		ptrShm = (char*) shmat(shmEscID, NULL, 0);
		if (ptrShm == (char*) -1) {
			fprintf(stderr, "Error al asignar el segmento de memoria del escritor\n");
			return -7;
		}
		
		productor(semEscID, ptrShm);

		shmdt(ptrShm);

	}

	kill(procesoLector, SIGTERM); // Si terminó antes el proceso lector, forzamos a que termine el proceso hijo
	waitpid(procesoLector, NULL, 0);

	// Limpiando recursos IPC
	borrarSemaforos(&semEscID, &semLecID);
	eliminarMemoriasCompartidas(&shmEscID, &shmLecID);

	return 0;
}


void manejoHijo(int) {
	terminar = 1;
}
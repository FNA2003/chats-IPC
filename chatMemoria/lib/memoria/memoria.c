#include "memoria.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int inicializarMemoria(key_t dirShm1, key_t dirShm2, int *shmID1, int *shmID2) {
	if (!shmID1 || !shmID2) return -1;
	
	// Trato de obtener (o crear si no existe) el segmento de memoria del bloque propio
	*shmID1 = shmget(dirShm1, 0, 0);
	if (*shmID1 < 0 && errno == ENOENT)
		*shmID1 = shmget(dirShm1, TAMANIO_MENSAJE, IPC_CREAT | 0600);
	if (*shmID1 < 0) {
		fprintf(stderr, "Error tratando de obtener/crear el primer segmento de memoria compartida");
		return -2;
	}
	
	// Misma pero para, la del otro proceso
	*shmID2 = shmget(dirShm2, 0, 0);
	if (*shmID2 < 0 && errno == ENOENT)
		*shmID2 = shmget(dirShm2, TAMANIO_MENSAJE, IPC_CREAT | 0600);
	if (*shmID2 < 0) {
		fprintf(stderr, "Error tratando de obtener/crear el segundo segmento de memoria compartida");
		return -3;
	}

	return 0;
}

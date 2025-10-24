#include "memoria.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int inicializarMemoriasCompartidas(key_t dirShm1, key_t dirShm2, int *shmID1, int *shmID2) {
	if (!shmID1 || !shmID2) return -1;
	
	// Trato de obtener (o crear si no existe) el segmento de memoria del bloque propio
	*shmID1 = shmget(dirShm1, TAMANIO_MENSAJE, 0);
	if (*shmID1 < 0 && errno == ENOENT)
		*shmID1 = shmget(dirShm1, TAMANIO_MENSAJE, IPC_CREAT | 0600);
	if (*shmID1 < 0) {
		perror("Error tratando de obtener/crear el primer segmento de memoria compartida");
		return -2;
	}
	
	// Misma pero para, la del otro proceso
	*shmID2 = shmget(dirShm2, TAMANIO_MENSAJE, 0);
	if (*shmID2 < 0 && errno == ENOENT)
		*shmID2 = shmget(dirShm2, TAMANIO_MENSAJE, IPC_CREAT | 0600);
	if (*shmID2 < 0) {
		perror("Error tratando de obtener/crear el segundo segmento de memoria compartida");
		return -3;
	}

	return 0;
}


int eliminarMemoriasCompartidas(int* shmId1, int* shmId2) {
	int retVal = 0;

	// Si hay un puntero a cualquiera de las dos memorias, trato de borrarlas
	
	if (shmId1) {
		retVal = shmctl(*shmId1, IPC_RMID, NULL);
		if (retVal < 0 && errno != EIDRM && errno != EINVAL) {
            perror("Error al tratar de borrar el primer segmento de memoria compartida!");
            return -1;
        }
	}
	if (shmId2) {
		retVal = shmctl(*shmId2, IPC_RMID, NULL);
		if (retVal < 0 && errno != EIDRM && errno != EINVAL) {
            perror("Error al tratar de borrar el segundo segmento de memoria compartida!");
            return -2;
        }
	}
	return 0;
}
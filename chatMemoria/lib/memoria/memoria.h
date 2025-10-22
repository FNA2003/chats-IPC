#ifndef MEMORIA_H
#define MEMORIA_H

#include <sys/types.h>

#define TAMANIO_MENSAJE 256

// Obtiene la dirección de los dos segmentos de memoria o retorna < 0 en caso de error
int inicializarMemoria(key_t dirShm1, key_t dirShm2, int *shmID1, int *shmID2);

#endif
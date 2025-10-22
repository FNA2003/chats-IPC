#ifndef MEMORIA_H
#define MEMORIA_H

#include <sys/types.h>

#define TAMANIO_MENSAJE 256

// Obtiene, o crea (si no existen), dos bloques de memoria
// compartida de clave "dirShm1" y "dirShm2", los id's de estos
// son almacenados en los enteros apuntados por "shmID1" y "shmID2".
// En caso de error retorna un número negativo, sino, retorna 0
int inicializarMemoriasCompartidas(key_t dirShm1, key_t dirShm2, int *shmID1, int *shmID2);

// Elimino la/s memorias compartidas señaladas por los parámetros
// En caso de que falle alguna de las operaciones, retorno < 0
// En caso de éxito, 0.
int eliminarMemoriasCompartidas(int* shmId1, int* shmId2);

#endif
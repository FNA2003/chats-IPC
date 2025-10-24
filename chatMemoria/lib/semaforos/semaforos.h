#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#include <sys/ipc.h>

// Señales V y P (respectivamente) de manejo de semáforos básica

void signal(int semId, int semNum);

void wait(int semId, int semNum);

int getSem(key_t semKeyEsc, key_t semKeyLec, int *semIDEsc, int *semIDLector);

int borrarSemaforos(int *semId1, int *semId2);

#endif
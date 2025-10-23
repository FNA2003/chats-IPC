#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#include <sys/types.h>

// Señales V y P (respectivamente) de manejo de semáforos básica

void signal(int semId, int semNum);

void wait(int semId, int semNum);

int getSem(key_t semkey1, key_t semkey2, int *semid1, int *semid2);

int borrarSemaforos(int *semId1, int *semId2);

#endif
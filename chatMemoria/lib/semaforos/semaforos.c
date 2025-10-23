#include "semaforos.h"
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>

void signal(int semId, int semNum) {
    struct sembuf sem_op; // Estructura propia para el trabajo con semop

    sem_op.sem_num = semNum; // Número dentro del 'array' de semáforos
    sem_op.sem_op = 1;       // Operación incremento (en este caso +1)
    sem_op.sem_flg = 0;      // Flag de, operación normal (bloqueante)
    
    semop(semId, &sem_op, 1); // Realizo una única operación (signal) sobre el semáforo
}

void wait(int semId, int semNum) {
    struct sembuf sem_op; // Estructura propia para el trabajo con semop

    sem_op.sem_num = semNum; // Número dentro del 'array' de semáforos
    sem_op.sem_op = -1;      // Operación decremento (en este caso -1)
    sem_op.sem_flg = 0;      // Flag de, operación normal (bloqueante)

    semop(semId, &sem_op, 1); // Realizo una única operación (wait) sobre el semáforo

}


int getSem(key_t semkey1, key_t semkey2, int *semid1, int *semid2) {
    if (!semid1 || !semid2) return -1;

    *semid1 = semget(semkey1, 1, 0);
    if (*semid1 < 0 && errno == ENOENT)
        *semid1 = semget(semkey1, 1, IPC_CREAT | 0600);
    if (*semid1 < 0) {
        perror("Error al obtener/crear el semáforo 1");
        return -2;
    }

    *semid2 = semget(semkey1, 1, 0);
    if (*semid2 < 0 && errno == ENOENT)
        *semid2 = semget(semkey2, 1, IPC_CREAT | 0600);
    if (*semid2 < 0) {
        perror("Error al obtener/crear el semáforo 2");
        return -3;
    }

    return 0;
}

int borrarSemaforos(int *semId1, int *semId2) {
    int retVal = 0;
    if (semId1)
        retVal += semctl(*semId1, 0, IPC_RMID);
    if (semId2)
        retVal += semctl(*semId2, 0, IPC_RMID);

    if (retVal < 0) { 
        fprintf(stderr, "Error al tratar de eliminar uno de los semáforos!\n");
        return -1;
    }

    return 0;
}
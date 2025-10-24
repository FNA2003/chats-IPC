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


int getSem(key_t semKeyEsc, key_t semKeyLec, int *semIDEsc, int *semIDLector) {
    if (!semIDEsc || !semIDLector) return -1;

    *semIDEsc = semget(semKeyEsc, 1, 0);
    if (*semIDEsc < 0 && errno == ENOENT)
        *semIDEsc = semget(semKeyEsc, 1, IPC_CREAT | 0600);
    if (*semIDEsc < 0) {
        perror("Error al obtener/crear el semáforo 1");
        return -2;
    }

    // Al semaforo de "aviso de escritura" lo inicializamos nosotros en 1
    // Para evitar que el otro poceso lo cree y trate de acceder a el
    if (semctl(*semIDEsc, 0, SETVAL, 1) < 0) { 
        perror("Error al inicializar el semáforo de escritura");
        return -3;
    }

    *semIDLector = semget(semKeyLec, 1, 0);
    if (*semIDLector < 0 && errno == ENOENT) {
        *semIDLector = semget(semKeyLec, 1, IPC_CREAT | 0600);
        
        if (semctl(*semIDLector, 0, SETVAL, 0) < 0) { // Por las dudas, el semaforo de lectura estará "suspendido"
            perror("Error al inicializar el semáforo de lectura");
            return -4;
        }
    }
    if (*semIDLector < 0) {
        perror("Error al obtener/crear el semáforo de lectura");
        return -5;
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
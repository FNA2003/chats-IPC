#include "semaforos.h"
#include <sys/sem.h>

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
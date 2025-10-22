#ifndef SEMAFOROS_H
#define SEMAFOROS_H

// Señales V y P (respectivamente) de manejo de semáforos básica

void signal(int semId, int semNum);

void wait(int semId, int semNum);

#endif
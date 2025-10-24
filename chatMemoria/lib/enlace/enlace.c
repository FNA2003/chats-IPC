#include "enlace.h"
#include "semaforos/semaforos.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Variable que usa main.c para recibir cuando finalizó el proceso lector (hijo)
// Para comunicarle al proceso padre y que deje procesar
// Proceso hijo (SIGUSR1)-> main.c { manejarHijo() - terminar = 1 } -> proceso principal, ejecutando main terminar = 0
extern volatile sig_atomic_t terminar;
                                

void productor(int semIDEscritura, char memoriaEscritura[TAMANIO_MENSAJE]) {
    char buffer[TAMANIO_MENSAJE];

    printf("Para finalizar, ingrese '%s'\n", PROTOCOLO_SALIDA);

    do{
        if (terminar) break; // Si el hijo leyó el protocolo de salida, termino al padre

        sem_wait(semIDEscritura, 0); // Hago el lockeo acá para evitar escribir si se está leyendo
        printf(">: ");

        // Obtengo la cadena que quiere poner en memoria el usuario
        fgets(buffer, TAMANIO_MENSAJE, stdin); // Nota; fgets() ya pone el '\0' al final
        buffer[strlen(buffer) - 1] = '\0'; // Borro el salto de linea

        // Escribo en la memoria compartida y libero el lockeo
        strcpy(memoriaEscritura, buffer);
        sem_signal(semIDEscritura, 0);
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)));
}
void consumidor(int semIDLectura, char memoriaLectura[TAMANIO_MENSAJE]) {
    char buffer[TAMANIO_MENSAJE];

    do {
        sem_wait(semIDLectura, 0); // Esperamos que se produzca algo
        strcpy(buffer, memoriaLectura);

        // Por seguridad, siempre verificamos si hay algo que leer
        if (strlen(buffer) > 0)
            printf("[USR]: %s\n", buffer);

        memset(memoriaLectura, '\0', TAMANIO_MENSAJE); // Lectura destructiva

        sem_signal(semIDLectura, 0); // 'Levanto el lock'
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)));


    kill(getppid(), SIGUSR1); // Aviso al proceso padre (escritor) que recibi protoclo de salida
}
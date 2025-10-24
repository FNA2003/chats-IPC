#include "enlace.h"
#include "semaforos.h"
#include <string.h>
#include <stdio.h>

void productor(int semIDEscritura, char memoriaEscritura[TAMANIO_MENSAJE]) {
    char buffer[TAMANIO_MENSAJE];

    do{
        wait(semIDEscritura, 0); // Hago el lockeo acá para evitar escribir si se está leyendo
        printf(">: ");

        // Obtengo la cadena que quiere poner en memoria el usuario
        fgets(buffer, TAMANIO_MENSAJE, stdin); // Nota; fgets() ya pone el '\0' al final

        // Escribo en la memoria compartida y libero el lockeo
        strcpy(memoriaEscritura, buffer);
        signal(semIDEscritura, 0);
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)));
}
void consumidor(int semIDLectura, char memoriaLectura[TAMANIO_MENSAJE]) {
    char buffer[TAMANIO_MENSAJE];

    do {
        wait(semIDLectura, 0); // Esperamos que se produzca algo
        strcpy(buffer, memoriaLectura);

        // Por seguridad, siempre verificamos si hay algo que leer
        if (strlen(buffer) > 0)
            printf("[USR]: %s\n", buffer);

        memset(memoriaLectura, '\0', TAMANIO_MENSAJE); // Lectura destructiva

        signal(semIDLectura, 0); // 'Levanto el lock'
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)));
}
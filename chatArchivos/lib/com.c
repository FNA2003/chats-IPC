#include "com.h"
#include <unistd.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void productor(int fdEscritura) {
    char buffer[NCARAC_MENSAJE];

    do {
        memset(buffer, '\0', NCARAC_MENSAJE);

        flock(fdEscritura, LOCK_EX); // Lockeo para evitar que se lea cuando vamos a escribir

        fgets(buffer, NCARAC_MENSAJE, stdin); // Tomo lo que se escriba por la entrada estándar
        buffer[strlen(buffer) - 1] = '\0'; // Saco el salto de línea

        // Intentamos escribir y manejos posibles fallos
        if (write(fdEscritura, buffer, NCARAC_MENSAJE) < 0) {
            // Si se borró el archivo o su referencia, finalizamos la producción
            if (errno == EINVAL || errno == EINTR) return;
            // Pero, si hubo otro error, lo mostramos antes de seguir
            else perror("Error de escritura");
        }

        flock(fdEscritura, LOCK_UN); // Libero el lockeo
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}

void consumidor(int fdLectura) {
    char buffer[NCARAC_MENSAJE];

    do {
        memset(buffer, '\0', NCARAC_MENSAJE);

        flock(fdLectura, LOCK_EX); // Lockeo para evitar que se borre lo que quiero leer

        // Intentamos leer y manejos posibles fallos
        if (read(fdLectura, buffer, NCARAC_MENSAJE) < 0) {
            // Si se borró el archivo o su referencia, finalizamos la lectura
            if (errno == EINVAL || errno == EINTR) return;
            // Pero, si hubo otro error, lo mostramos antes de seguir
            else perror("Error de lectura!");
        }

        // Si hay algo escrito, lo leemos
        if (strlen(buffer) > 0)
            printf("[USUARIO]: %s\n", buffer);

        flock(fdLectura, LOCK_UN); // Libero el lockeo
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}
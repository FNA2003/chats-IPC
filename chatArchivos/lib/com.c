#include "com.h"
#include <unistd.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern sig_atomic_t ejecutar;


void productor(FILE *fEscritura) {
    printf("Chat iniciado. Para salir, escriba '%s'\n", PROTOCOLO_SALIDA);

    char buffer[NCARAC_MENSAJE];

    do {
        if (!ejecutar) break; // Si el consumidor terminó, salimos

        memset(buffer, '\0', NCARAC_MENSAJE);

        // Recibimos caracteres por entrada estandar (o, hasta '\n')
        printf(">: ");
        fgets(buffer, NCARAC_MENSAJE, stdin);
        buffer[strlen(buffer) - 2] = '\n'; // Nos tenemos que asegurar que se termine
                                           // con '\n' para respetar el protocolo.
                                           // Además fgets() ya introduce el '\0', no lo alteramos.

        // Escribimos el mensaje en el archivo
        if (fwrite(buffer, strlen(buffer), 1, fEscritura) != 1) {
            if (errno == EINVAL || errno == EINTR) return;
            else perror("Error de escritura");
        }

        fflush(fEscritura); // Forzamos la escritura en el disco (forzamos el buffer interno)

    } while (strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}


void consumidor(FILE *fLectura) {
    char buffer[NCARAC_MENSAJE];

    do {
        memset(buffer, '\0', NCARAC_MENSAJE);

        // Intentamos leer una línea del archivo
        if (fgets(buffer, NCARAC_MENSAJE, fLectura)) {
            printf("[USUARIO]: %s", buffer);
        } else {
            clearerr(fLectura); // Limpiamos EOF si se alcanzó
            usleep(100000);     // Esperamos 100ms para evitar busy wait
        }

    } while (strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);

    // Avisamos al padre que terminamos
    kill(getppid(), SIGUSR1);
}

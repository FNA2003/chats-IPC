#include "com.h"
#include <unistd.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

extern sig_atomic_t ejecutar;

// Nota, usamos el '\n' como delimitador

void productor(FILE *fEscritura) {
    printf("Configurado, para dejar de enviar mensajes ingrese '%s'\n", PROTOCOLO_SALIDA);

    char buffer[NCARAC_MENSAJE];

    do {
        if(!ejecutar) break; // Se recibió la señal que el padre debe terminar

        memset(buffer, '\0', NCARAC_MENSAJE);

        // Tomo lo que se escriba por la entrada estándar, incluido el '\n'
        printf(">: ");
        fgets(buffer, NCARAC_MENSAJE, stdin);

        // Intentamos escribir y manejos posibles fallos
        if (fwrite(buffer, strlen(buffer), 1, fEscritura) != 1) {
            // Si se borró el archivo o su referencia, finalizamos la producción
            if (errno == EINVAL || errno == EINTR) return;
            // Pero, si hubo otro error, lo mostramos antes de seguir
            else perror("Error de escritura");
        }
        
        fflush(fEscritura); // Forzamos a que pase el buffer a disco

    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);
}

void consumidor(FILE *fLectura) {
    char buffer[NCARAC_MENSAJE];

    do {
        memset(buffer, '\0', NCARAC_MENSAJE);

        // Si puedo, obtengo el mensaje
        if (fgets(buffer, NCARAC_MENSAJE, fLectura))
            printf("[USUARIO]: %s", buffer);
        // Si falló la lectura, reseteo las flags para que la próxima lectura lea sin fallar
        else
            clearerr(fLectura);
    } while(strncmp(buffer, PROTOCOLO_SALIDA, strlen(PROTOCOLO_SALIDA)) != 0);

    kill(getppid(), SIGUSR1);
}
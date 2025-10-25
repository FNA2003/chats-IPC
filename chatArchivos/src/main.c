#include "com.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>

// Variable compartida para controlar la ejecución del productor
sig_atomic_t ejecutar = 1;

// Manejador de señal que indica que el consumidor terminó
void manejarHijo(int) {
    ejecutar = 0;
}

int main(int argc, char *argv[]) {
    // Validación de argumentos
    if (argc != 3) {
        fprintf(stderr, "Uso:\n\t./chatMemoria <canal_escritura> <canal_lectura>\n");
        return -1;
    }

    int canalEsc = atoi(argv[1]);
    int canalLec = atoi(argv[2]);

    if (canalEsc < 0 || canalLec < 0 || canalEsc == canalLec) {
        fprintf(stderr, "Los canales tienen que ser positivos y distintos.\n");
        return -2;
    }

    // Nombres de archivo para los canales
    char nomEscritura[40], nomLectura[40];
    sprintf(nomEscritura, "canal_%d.log", canalEsc);
    sprintf(nomLectura, "canal_%d.log", canalLec);

    // Abrimos los archivos en modo escritura/lectura binaria
    FILE *ptrEscritura = fopen(nomEscritura, "w+b");
    if (!ptrEscritura) {
        perror("Error al abrir canal de escritura");
        return -3;
    }

    FILE *ptrLectura = fopen(nomLectura, "w+b");
    if (!ptrLectura) {
        perror("Error al abrir canal de lectura");
        fclose(ptrEscritura);
        return -4;
    }

    // Configuramos el manejador de señal para detectar fin del consumidor
    signal(SIGUSR1, manejarHijo);

    // Creamos el proceso hijo (consumidor)
    pid_t procesoHijo = fork();
    if (procesoHijo < 0) {
        perror("Error al crear proceso hijo");
        fclose(ptrEscritura);
        fclose(ptrLectura);
        return -5;
    }

    if (procesoHijo == 0) {
        // Proceso hijo: consumidor
        consumidor(ptrLectura);
    } else {
        // Proceso padre: productor
        productor(ptrEscritura);
        kill(procesoHijo, SIGTERM); // Finalizamos el hijo si nosotros escribimos en el log el "bye"
    }

    // Esperamos que el hijo termine
    wait(NULL);

    // Cerramos y eliminamos los archivos
    fclose(ptrEscritura);
    fclose(ptrLectura);
    remove(nomEscritura);
    remove(nomLectura);

    return 0;
}

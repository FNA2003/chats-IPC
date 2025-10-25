#include "com.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>


// Variable atómica que usará el proceso hijo (consumidor) para avisar que terminó su ejecución
// y, debe dejar de leer
sig_atomic_t ejecutar = 1;
void manejarHijo(int) {
    ejecutar = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso:\n\t./chatMemoria <int:numero de canal escritura> <int: numero de canal lectura>\n");
        return -1;
    }
    int canalEsc, canalLec;
    FILE *ptrEscritura;
    FILE *ptrLectura;

    pid_t procesoHijo;

    char auxiliar[40];


    signal(SIGUSR1, manejarHijo); // Aceptar cuando terminó el hijo

    // Tratamos de castear los canales recibidos a entero
    canalEsc = atoi(argv[1]);
    canalLec = atoi(argv[2]);

    if (canalEsc < 0 || canalLec < 0 || canalEsc == canalLec) {
        fprintf(stderr, "Debe indicar un número de canal positivo y distinto para cada operación!\n");
        return -2;
    }

    // Le damos el nombre de canal de escritura
    sprintf(auxiliar, "canal_%d.log", canalEsc);
    // Y, tratamos de crearlo y abrirlo
    ptrEscritura = fopen(auxiliar, "w+b");
    if (!ptrEscritura) {
        perror("Error al tratar de obtener el archivo para la escritura");
        return -3;
    }

    // Nombre de canal de lectura
    sprintf(auxiliar, "canal_%d.log", canalLec);
    // Lo tratamos de abrir (o crear si no existe ~ el otro proceso no fué creado aún)
    ptrLectura = fopen(auxiliar, "w+b");
    if (!ptrLectura) {
        perror("Error al tratar de obtener el archivo para la lectura!");
        fclose(ptrEscritura);
        return -4;
    }


    procesoHijo = fork();
    if (procesoHijo < 0) {
        perror("Error creando el proceso hijo (lector)");
        fclose(ptrEscritura);
        fclose(ptrLectura);
        return -5;
    }

    if (procesoHijo == 0) {
        consumidor(ptrLectura);
    }
    else {
        productor(ptrEscritura);
        kill(procesoHijo, SIGTERM); // Forzamos a que termine el hijo
    }

    wait(NULL);

    fclose(ptrEscritura);
    fclose(ptrLectura);

    return 0;
}
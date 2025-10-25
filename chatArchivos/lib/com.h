#ifndef COM_H
#define COM_H
#include <stdio.h>

#define PROTOCOLO_SALIDA "bye"
#define NCARAC_MENSAJE 256

void productor(FILE *fEscritura);

void consumidor(FILE *fLectura);

#endif
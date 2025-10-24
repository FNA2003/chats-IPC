#ifndef ENLACE_H
#define ENLACE_H

#include "memoria/memoria.h"

#define PROTOCOLO_SALIDA "bye"

void productor(int semIDEscritura, char memoriaEscritura[TAMANIO_MENSAJE]);
void consumidor(int semIDLectura, char memoriaLectura[TAMANIO_MENSAJE]);

#endif
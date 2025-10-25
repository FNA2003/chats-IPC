#ifndef COM_H
#define COM_H


#define PROTOCOLO_SALIDA "bye"
#define NCARAC_MENSAJE 256

void productor(int fdEscritura);

void consumidor(int fdLectura);

#endif
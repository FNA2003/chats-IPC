/*
Debés implementar un sistema de chat entre procesos, pero esta vez usando colas de mensajes en lugar de FIFO. El esquema de procesos debe ser el mismo que en el punto anterior: dos procesos independientes, y cada uno debe crear un proceso hijo.

Podés resolver todo con una sola cola de mensajes compartida entre los procesos. Para que funcione correctamente, vas a necesitar al menos dos tipos distintos de mensajes, por ejemplo, uno para cada dirección de comunicación (de A a B y de B a A).

La solución puede estar en un único programa, que se ejecuta desde dos terminales distintas con argumentos por línea de comandos para configurar cada proceso. Es decir, no necesitás escribir dos programas distintos, sino que el mismo programa debe poder comportarse como uno u otro proceso según los argumentos que reciba.
*/

#include <sys/msg.h> // Librería de cola de mensajes
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE

typedef struct {
	long type; 				 // Tipo de mensaje, requerido por sys/msg.h
	char mtext[BUFFER_SIZE]; // Mensaje a enviar, en nuestro caso, un arreglo de char's
} msgbuf;


int main(int argc, char *argv[]) {
	int c_id = msgget(0xa, IPC_CREAT | IPC_EXCL | 0600);
	printf("Cola de mensaje id: %d", c_id);
	msgctl(c_id, IPC_RMID, NULL);
}

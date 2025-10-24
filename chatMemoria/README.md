# Chat entre procesos con memoria compartida y semáforos (IPC)

Este programa permite establecer una comunicación tipo "chat" entre dos procesos independientes utilizando **memoria compartida** y **semafos** mediante IPC System V. Cada proceso tiene su propio espacio de escritura y su semáforo de control, mientras que el otro proceso actúa como lector de ese espacio.

## Arquitectura

### Espacios de memoria

- Se crean **dos bloques** de memoria compartida, cada uno de 256 bytes.
- El _proceso A_ escribe en el primer bloque, y el _proceso B_ en el segundo.
- Cada proceso lector accede al bloque de memoria del otro proceso.

### Semáforos

- Se utilizan **dos semáforos** para garantizar la sincronización entre escritura y lectura:
> **Semáforo 1**: sincroniza la escritura de A con la lectura del hijo de B.
> **Semáforo 2**: sincroniza la escritura de B con la lectura del hijo de A.


## Comunicación

### Inicialización

- Cada proceso, trata de tomar el recurso, si no los halla, los crea.
> Como medida de seguridad, solamente el proceso que usa el semáforo para comunicar la escritura puede darle un valor positivo a este. En la creación del semáforo de lectura, este se inicializa en 0 (Evitando el acceso).

### Pseudo-código ejecución

#### Proceso Escritor A

```c

while (Produccion_A) {
    wait(Semaforo_1);
    // Escribo mensaje en memoria
    ...
    signal(Semaforo_1);
}
```

#### Proceso Lector B (hijo de B)

```c
while (Consumo_A) {
    wait(Semaforo_1);
    if (mensaje disponible) {
        // Mostrar mensaje
        ...
    }
    signal(Semaforo_1);
}
```

Este patrón se repite para el proceso B y el lector, hijo de A.


## Diseño

- Si el lector intenta leer antes de que el escritor haya escrito, no mostrará nada. De la misma forma que si no se envió nada.
- Cada semáforo garantiza _exclusión mutua_ entre escritura y lectura, evitando condiciones de carrera.
- La _separación de segmentos de memoria_ evita que los escritores se pisen entre sí. En caso de querer usar un único espacio de memoria común, se deve agregar otro semáforo que comunique a los procesos escritores.
- Si el lector detecta el mensaje ```"bye"``` (definido en ```lib/enlace/enlace.h```), envía una señal al proceso padre para finalizar el chat.
- Ambos procesos intentan eliminar los recursos IPC al finalizar, aunque ya hayan sido eliminados por el otro (atendiendo a esta excepción).


## Uso

### Compilación
1. Posicionecé en la dirección de este readme y, usando el programa ```make``` (verifique su instalación), ejecute:
```bash
make all
```
2. Esto creará una carpeta ```build/``` que contendrá el archivo compilado de este proyecto (```build/chatMemoria.c```).

### Ejecución
Luego de compilar el programa, procure tener dos consolas en el directorio ```build/``` de este proyecto y ejecute:
```bash
./chatMemoria <claveEscritura> <claveLectura>
```

- Las claves deben ser valores **decimales** que, se convertirán internamente en claves IPC (```key_t```) para los semáforos y la memoria compartida (esta clave se usará para ambos, procure tener una clave en su IPC que le permita tener semáforos y memoria compartida con estas).
- Cada proceso debe recibir su par de claves correspondiente, es decir, la clave de escritura del primer proceso será la de lectura del segundo y de misma forma para la de lectura del primero.
- El programa maneja automáticamente la creación, inicialización y eliminación de los recursos IPC.


## Potencia y escalabilidad

Este programa permite generar **n pares de procesos independientes**, cada uno con sus propios recursos IPC, sin interferencia entre ellos. Está limitado únicamente por el sistema operativo en cuanto a recursos IPC disponibles.

> ⚠️ No se recomienda usar más de dos procesos sobre el mismo recurso, ya que la sincronización y la finalización se vuelven **no deterministas** (lectura no garantizada, pues se hizo destructiva... Puede alterarse para que no falle, BAJO SUS PROPIOS RIESGOS, en ```lib/enlace/enlace.c -> void consumidor(...){...}```).

## Estructura del proyecto

- ```src/main.c```: proceso principal, maneja fork, setup y limpieza.
- ```lib/memoria/```: funciones para inicializar y eliminar memoria compartida.
- ```lib/semaforos/```: funciones para manejar semáforos (```wait```, ```signal```, creación y eliminación).
- ```lib/enlace/```: funciones ```productor()``` y ```consumidor()``` que implementan el ciclo de chat.
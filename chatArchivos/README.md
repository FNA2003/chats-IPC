# 🗨️ Chat por Archivos

## Descripción del proyecto

Este programa implementa un sistema de chat entre dos procesos utilizando **archivos como medio de comunicación**. Cada proceso puede enviar y recibir mensajes a través de archivos binarios, simulando un canal de entrada y otro de salida. La comunicación es asincrónica y persistente, y se gestiona mediante lectura/escritura secuencial con sincronización básica.


## Diseño

### Comunicación

- El proceso principal crea un **proceso hijo** que actúa como consumidor (lector).
- El proceso padre actúa como productor (escritor).
- La lectura se realiza línea por línea (```fgets()```), y la escritura se hace con ```fwrite()``` seguido de ```fflush()``` para asegurar persistencia inmediata.
- Se utiliza una señal (```SIGUSR1```) para indicar que el consumidor ha finalizado.

### Sincronización

- Para evitar consumo excesivo de CPU, el consumidor utiliza ```usleep(100000)``` cuando no hay datos nuevos.
- Se emplea ```clearerr()``` para limpiar el estado de EOF y permitir nuevas lecturas.


## Compilación y ejecución

### Compilación

```bash
make
```

O manualmente:

```bash
gcc -o chatArchivos.out src/main.c lib/com.c -Wall -Wextra
```

### Ejecución

Cada instancia del programa representa un usuario. Se deben ejecutar **dos procesos** con canales cruzados:

```bash
./chatArchivos.out 1 2   # Usuario A: escribe en canal 1, lee de canal 2
./chatArchivos.out 2 1   # Usuario B: escribe en canal 2, lee de canal 1
```


## Protocolo de salida

Para finalizar el chat, se debe escribir la palabra clave definida en ```PROTOCOLO_SALIDA``` (por defecto ```"bye"```). Esto termina la sesión y elimina los archivos utilizados.

## Estructuras de datos utilizadas

- Buffers de caracteres (```char buffer[NCARAC_MENSAJE]```) para almacenar mensajes.
- Archivos binarios para la comunicación (se les agregó la extensión ```.log``` para poder abrirlos con cualquier visualizar de texto sin problema).
- Señal (```SIGUSR1```) para notificación de fin de lectura.


## Ejemplo de uso

```bash
Terminal 1:
./chatArchivos.out 1 2
>: Hola!
>: Cómo estás?
>: bye

Terminal 2:
./chatArchivos.out 2 1
[USUARIO]: Hola!
[USUARIO]: Cómo estás?
[USUARIO]: bye
```

### Limitaciones
- El _proceso lector_ tiene una latencia propia de 100ms para evitar una busy wait. Es recomendable que se cambie esta espera, y se sincronice con semáforos o con ```inotify``` para modificación del archivo.
- No se podrá leer ninguno de los mensajes hasta que **los dos procesos ejecuten el programa**.
- Una vez que uno de los usuarios introduzca el protocolo de salida (véase en ```lib/com.h```), se borrarán los archivos usados para la comunicación. Por un lado, esto implica que solamente habrá dos puntos en un mismo conjunto de canales (por cada canal de escritura y lectura, habrá un único par de proceso en el chat) y, por otro, significa que no podrá ver el log creado por el chat.
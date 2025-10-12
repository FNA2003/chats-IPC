# Chat entre Procesos con FIFO

 Este proyecto implementa un chat entre dos procesos independientes utilizando FIFO (named pipes) en sistemas Unix/Linux. Cada proceso se comunica a través de dos FIFOs: uno para escritura y otro para lectura.

## Estructura del Proyecto

- ```chatfifo.c```: Código fuente del programa que implementa el chat.

## Compilación

Compilar con ```gcc```:

```
gcc -o chatfifo chatfifo.c
```

## Ejecución
1. Crear los FIFO's desde línea de comandos:
```
mkfifo <fifo1>
mkfifo <fifo2>
```
2. Abrir dos terminales (cada una ubicada donde se creo cada fifo respectivamente) y ejecutar el programa con los _"parámetros invertidos"_:

**Terminal 1**
```
./chatfifo <fifo1> <fifo2>
```

**Terminal 2**
```
./chatfifo <fifo2> <fifo1>
```

3. Si desea salir del chat, en cualquiera de los dos procesos ingrese: ```bye```

4. Al finalizar, elimine los fifo's creados con:
```
unlink <fifo1>
unlink <fifo2>
```

### Detalles
- El programa recibe dos argumentos por línea de comandos:
> Primer argumento: nombre del FIFO de escritura.
> Segundo argumento: nombre del FIFO de lectura.
- El protocolo de salida se basa en la palabra clave bye.

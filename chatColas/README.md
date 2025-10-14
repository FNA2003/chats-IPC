# 📨 Chat entre procesos con Cola de Mensajes (System V)

## 📌 Descripción

Este programa implementa un sistema de chat entre dos procesos independientes utilizando una única cola de mensajes System V. Cada proceso se ejecuta desde una terminal distinta y se comunica enviando y recibiendo mensajes de diferentes tipos dentro de la misma cola.

El programa crea dos procesos internos:
- Un proceso hijo que se encarga de **leer** mensajes.
- Un proceso padre que se encarga de **escribir** mensajes.

La comunicación se mantiene hasta que uno de los usuarios escribe la palabra clave ```bye```, lo que desencadena la finalización del chat.

---

## 🛠️ Instrucciones de compilación y ejecución

1. Primero, compila, para esto, usá ```gcc```:

```
gcc -o chatColas chatColas.c
```

2. Abrí dos terminales.

3. En la primera terminal, ejecutá:

```
./chatColas 1 2
```

4. En la segunda terminal, ejecutá:

```
./chatColas 2 1
```

> Cada proceso leerá mensajes del tipo indicado por el primer argumento y escribirá mensajes del tipo indicado por el segundo argumento.

5. Ahora, podes escribir desde cualquiera de las dos terminales y ver la salida en la otra.

6. Para salir del chat, escribí ```bye``` en cualquiera de las terminales. Esto cerrará el proceso lector y notificará al proceso escritor para que también finalice.

---

## ⚙️ Detalles técnicos

- **Cola de mensajes**: Se utiliza una única cola identificada por la clave ```0xA```. Los mensajes se diferencian por su campo ```type```.
- **Estructura de mensaje**: Se define una estructura ```msgbuf``` con un campo ```type``` y un buffer de texto.
- **Procesos**:
  - El proceso padre escribe mensajes en la cola.
  - El proceso hijo lee mensajes de la cola.
- **Finalización**:
  - Cuando se recibe el mensaje ```"bye"```, el proceso lector termina.
  - El proceso escritor detecta la finalización del hijo mediante la señal ```SIGCHLD``` y deja de escribir.
- **Limpieza**:
  - Al finalizar, se intenta eliminar la cola de mensajes con ```msgctl(..., IPC_RMID)```. Solo uno de los procesos logrará borrarla (el otro fallará _silenciosamente_).

---

## 📝 Notas

- Si queres cambiar la clave de cola de mensajes o la palabra de salida, se hallan definidas al principio del programa.
- El programa no maneja múltiples usuarios ni concurrencia avanzada. Está diseñado como una demostración básica de IPC con colas de mensajes.
- Cabe decir que las **"Message Queue"** son una herramienta muy potente que provee la API de UNIX, por esto, con simplemente modificar cuando se borra la cola (cuando no hayan mensajes en la cola o procesos en esta) con **ÚNICO** _programa_ y _queue_ podemos tener un chat entre _n_ procesos (siempre y cuando los identificadores de lectura y escritura seán distintos entre cada par de programas)

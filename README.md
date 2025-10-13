# 💬 chatsIPC

Este repositorio contiene distintas implementaciones de un sistema de chat entre procesos independientes, utilizando diferentes mecanismos de comunicación provistos por UNIX (precisamente, SVR4).

Cada solución se encuentra organizada en su propia carpeta, con su respectivo código y documentación.

## 📁 Estructura del repositorio

- `chatFifos/` – Chat implementado mediante **FIFO (named pipes)**
- `chatColas/` – Chat implementado mediante **Colas de Mensajes**
- `chatMemoria/` – Chat implementado mediante **Memoria Compartida y Semáforos**
- `chatArchivos/` – Chat implementado mediante **Archivos**

## 🧠 Objetivo

Explorar y comparar distintas técnicas de comunicación entre procesos (IPC) en sistemas operativos tipo Unix/Linux, mediante la implementación de un chat funcional en cada caso.

Cada solución parte de un mismo esquema: dos procesos independientes que intercambian mensajes, con protocolos definidos para iniciar y finalizar la comunicación.

## 📌 Requisitos generales

- UNIX en una versión superior a la que implementó POSIX y SVR4.
- Compilador C (e.g., `gcc`)
- Permisos para crear recursos IPC (pipes, colas, memoria compartida, etc.)

## 📚 Documentación

Cada carpeta contiene un archivo `README.md` con:
- Descripción
- Instrucciones de compilación y ejecución
- Detalles técnicos y algunas notas

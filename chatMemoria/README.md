- Espacios de memoria:
> Habrá dos espacios de memoria correspondientes a un mensaje (de 256 bytes cada una), el espacio del primer mensaje le corresponde al proceso A (para su escritura) y el segundo espacio del segundo mensaje al proceso B (para su escritura).

- Semáforos:
> El Semáforo 1 sincroniza la escritura del proceso A con la lectura del proceso hijo de B.
> El Semáforo 2 sincroniza la escritura del proceso B con la lectura del proceso hijo de A.

---

> **Notas:** Se deben inicializar los semáforos en bloqueados (0), luego, antes de comenzar el _"chat"_, el proceso escritor de cada uno, debe incrementarlo, véase:

1. Proceso Escritor A:
```
signal(Semaforo_1)
mientras(Produccion_A):
	wait(Semaforo_1)
	// Escribo mensaje en memoria
	...
	signal(Semaforo_1)
	...
```

2. Proceso Lector B (hijo de B):
```
...
mientras(Consumo_A):
    wait(Semaforo_1)

    si(Consumo_A no es vacío):
        // Mostrar mensaje
        ...

    signal(Semaforo_1)
```

## Advertencias de diseño:

- Si el lector intenta leer antes de que el escritor haya escrito, simplemente no mostrará nada.
- El semáforo garantiza exclusión mutua entre escritura y lectura, evitando condiciones de carrera.
- La separación de buffers evita que los escritores se pisen entre sí.

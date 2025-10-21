# Compilador y sus flags
CC = gcc
CFLAGS = -Ilib -Wall -Wextra

# Rutas de compilables
SRC = src/main.c

# Nombre del compilado
TAR = chatMemoria.out

# Ruta de compilación
BUILD = build/

# Pequeña "macro" para imprimir un mensaje que identifique la salida del Makefile en lugar de gcc u otro
MSG = "\033[37;44mMakefile\033[0m"


# Creo la dirección de compilación si es que no existe
$(BUILD):
	@echo $(MSG)
	@echo "La carpeta $(BUILD), no existe, creandola..."
	@mkdir -p $(BUILD)
	@echo "Directorio $(BUILD) creado!"

# Che, amigo, capaz que podríamos usar varios steps de compilación
# o usar .o así no tarda tanto cada compilación
all: $(BUILD)
	@echo $(MSG)
	@echo "Compilando el programa..."
	$(CC) $(SRC) $(CFLAGS) -o $(BUILD)$(TAR)
	@echo "Compilación exitosa\n\tDirección del archivo: $(BUILD)$(TAR)"

# Definir variáveis
CC = gcc
CFLAGS = -Wall -std=c17
SRC_DIR = src
OBJ_DIR = bin
INCLUDE_DIR = include

# Arquivos fonte
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/adt.c
# Arquivos objeto correspondentes
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# Nome do executável
EXEC = $(OBJ_DIR)/projeto

# Regra padrão
all: $(EXEC)

# Regra para compilar o executável
$(EXEC): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(EXEC)

# Regra para compilar arquivos .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) $< -o $@

# Limpar arquivos gerados
clean:
	rm -f $(OBJ_DIR)/*.o $(EXEC)


PRJ_DIR := $(shell basename "$(CURDIR)")

BIN_DIR := .
INC_DIR := inc
LIB_DIR := lib
OBJ_DIR := obj
SRC_DIR := src
OUT_DIR := out
SCRIPTS_DIR := scripts

EXE := $(BIN_DIR)/$(PRJ_DIR)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC       := gcc
CPPFLAGS := -I. -I$(SRC_DIR) -I$(INC_DIR) -MMD -MP
CFLAGS   := -Wall -Wextra -O2 -std=c17 -g3
LDFLAGS  := -L$(LIB_DIR)
LDLIBS   := -lm -pthread

all: prepare $(EXE)

prepare:
	mkdir -p $(OUT_DIR) $(SCRIPTS_DIR)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean prepare

clean:
	-@$(RM) -rv $(EXE) $(OBJ_DIR) $(OUT_DIR)

-include $(OBJ:.o=.d)
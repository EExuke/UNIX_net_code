# ==========================================
# © 2020 EExuke. All Rights Reserved.
# ==========================================

DIR_OBJ = ./obj
DIR_SRC = ./src
INC_SRC = ./inc

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c, ${DIR_OBJ}/%.o, $(notdir $(SRC)))

BIN_TARGET = unp_main_test

CC = gcc
CFLAGS = -g -Wall -I $(INC_SRC)
${BIN_TARGET}: ${OBJ}
	$(warning Building $(BIN_TARGET) ...)
	@$(CC) $(OBJ) -o $@

${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(warning Comping $< ...)
	@$(CC) $(CFLAGS) -c $< -o $@

# Prevent naming conflicts such as the existence of a file named clean.
.PHONY: clean

clean:
	@find ${DIR_OBJ} -name *.o -exec rm -rf {} \;
	@rm -rf $(BIN_TARGET)


# ==========================================
# © 2020 EExuke. All Rights Reserved.
# ==========================================

DIR_OBJ = .
DIR_SRC = .
INC_SRC += ../unp/inc

SRC = $(wildcard ${DIR_SRC}/*.c)
OBJ = $(patsubst %.c, ${DIR_OBJ}/%.o, $(notdir $(SRC)))

LIBS += -lpthread

BIN_TARGET = test_cli

CC = gcc
CFLAGS = -g -Wall -I $(INC_SRC)
${BIN_TARGET}: ${OBJ}
	$(warning Building $(BIN_TARGET) ...)
	@$(CC) $(OBJ) -o $@ ${LIBS}

${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(warning Comping $< ...)
	@$(CC) $(CFLAGS) -c $< -o $@

# Prevent naming conflicts such as the existence of a file named clean.
.PHONY: clean

clean:
	@rm ${DIR_OBJ}/*.o;
	@rm -rf $(BIN_TARGET)


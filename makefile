CC = gcc
CFLAGS = -Wall -Werror
C_DIR = src
H_DIR = include
HEADER = $(wildcard $(H_DIR)/*.h)
SRC_SERVER = $(wildcard $(C_DIR)/*.c) server.c
OBJ_SERVER = $(SRC_SERVER:.c=.o)
SRC_CLIENT = $(wildcard $(C_DIR)/*.c) client.c
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
EXEC = server client

all: $(EXEC)

server: $(OBJ_SERVER)
	@$(CC) -o $@ $^

server.o: $(HEADER)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

client: $(OBJ_CLIENT)
	@$(CC) -o $@ $^

client.o: $(HEADER)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean mrproper

clean:
	@rm -rf $(C_DIR)/*.o *.o

mrproper: clean
	@rm -rf $(EXEC)

EXEC = client
C_DIR = ../src
H_DIR = ../include
SRC = $(wildcard $(C_DIR)/*.c) *.c 
HEADER = $(wildcard $(H_DIR)/*.h)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	@rm -rf $(C_DIR)/*.o *.o 

mrproper: clean
	@rm -rf $(EXEC)


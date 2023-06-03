#CC= gcc #
CFLAGS= --std=c11  -pedantic -pedantic-errors -Wall -Wextra -Werror  -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L  -g
LDFLAGS= -pthread

#Si se agrega un directorio bajo src agregar una variable _DIR al final de estas siguiendo el patron
SRC_DIR= src
BMP_DIR= $(SRC_DIR)/Bmp
SHADOW_GEN_DIR= $(SRC_DIR)/ShadowGenerator


SS_EXEC= ss

#Si se agrega un directorio para compilar agregar un SRC+= al final de estas siguiendo el patron
SRC= $(wildcard $(SRC_DIR)/*.c)
SRC+= $(wildcard $(BMP_DIR)/*.c)
SRC+= $(wildcard $(SHADOW_GEN_DIR)/*.c)


#BMP_SRC = $(wildcard $(BMP_DIR)/*.c)


OBJ= $(patsubst %.c, %.o, $(SRC))
#BMP_OBJ= $(patsubst %.c, %.o, $(BMP_SRC))

.PHONY: all
all: ss

.PHONY: ss
ss: $(OBJ) $(BMP_SRC)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(SS_EXEC) $(OBJ)
#	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(SS_EXEC) $(OBJ) $(BMP_OBJ)

%.o : %.c %.h
	@$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f $(OBJ) $(SS_EXEC)
#find . -name '*.o' -type f -delete
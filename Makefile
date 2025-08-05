CC=gcc
EXEC=program.out
GRUPO=G1
NTAR=2

SRC_DIR=src
OBJ_DIR=obj
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
INCLUDE=-I./incs/
LIBS=-lallegro -lallegro_primitives -lallegro_image -lm -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf

CFLAGS=-Wall -Wextra -Wpedantic -O3
LDFLAGS= $(LIBS)

all: folders $(OBJ_FILES)
	$(CC) $(CFLAGS) -o build/$(EXEC) $(OBJ_FILES) $(INCLUDE) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INCLUDE)

.PHONY: clean folders send
clean:
	rm -f $(OBJ_FILES)
	rm -f build/$(EXEC)

folders:
	mkdir -p src obj incs build docs

send:
	tar czf $(GRUPO)-$(NTAR).tgz --transform 's,^,$(GRUPO)-$(NTAR)/,' Makefile src incs docs

# Regla para correr el programa sin tener que ir a build
run: all
	./build/$(EXEC)
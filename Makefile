CC ?= clang
CFLAGS ?= -Wall -Wextra -Wpedantic -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

SRC = src/vault.c src/vault_table.c src/vault_extract.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean test

all: tests/vault_smoke

tests/vault_smoke: tests/vault_smoke.c $(OBJ)
	$(CC) $(CFLAGS) -I. $^ -o $@

test: tests/vault_smoke
	./tests/vault_smoke

clean:
	rm -f src/*.o tests/vault_smoke

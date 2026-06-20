#!/bin/bash -eu

$CC $CFLAGS -I"$SRC" -c "$SRC/src/vault.c" -o vault.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/vault_table.c" -o vault_table.o
$CC $CFLAGS -I"$SRC" -c "$SRC/src/vault_extract.c" -o vault_extract.o

$CXX $CXXFLAGS -I"$SRC" \
  "$SRC/fuzz/vault_fuzzer.cc" \
  vault.o vault_table.o vault_extract.o \
  $LIB_FUZZING_ENGINE \
  -o "$OUT/vault_fuzzer"

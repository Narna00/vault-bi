# Vaultbox

Vaultbox is a small C library for reading a compact binary archive format. It
keeps parsed entries in a session table and supports simple scripted extraction
operations over stored records.

## Archive Format

Archives start with an 8-byte header:

```text
offset  size  field
0       4     magic: VLT1
4       2     little-endian version, currently 1
6       2     little-endian record count
```

Each record has a 5-byte record header followed by its body:

```text
offset  size  field
0       1     record type
1       4     little-endian body length
5       n     body
```

Record types:

- `0x10`: store an entry in the session table
- `0x20`: run an extraction script against a stored entry
- `0x30`: create an alias entry from an existing entry

## Repository Layout

```text
src/
  vault.c
  vault.h
  vault_extract.c
  vault_internal.h
  vault_table.c
fuzz/
  vault_fuzzer.cc
  dictionary.txt
  corpus/
tests/
  vault_smoke.c
.clusterfuzzlite/
  build.sh
  project.yaml
Makefile
```

The files in `fuzz/corpus/` are raw binary seed inputs for the fuzzer. The
files in `pocs/` are raw binary inputs used for deterministic sanitizer
validation.

## Build

```bash
make test
```

The ClusterFuzzLite build entrypoint is `.clusterfuzzlite/build.sh`. It writes
the fuzz target binary to `$OUT/vault_fuzzer`.

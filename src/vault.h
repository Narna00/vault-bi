#ifndef VAULTBOX_VAULT_H
#define VAULTBOX_VAULT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VAULT_OK = 0,
    VAULT_ERR_FORMAT = -1,
    VAULT_ERR_BOUNDS = -2,
    VAULT_ERR_NOMEM = -3
} VaultStatus;

typedef struct {
    uint16_t id;
    uint8_t flags;
    uint32_t size;
    uint8_t *data;
} VaultEntry;

typedef struct {
    VaultEntry *entries;
    size_t count;
    size_t capacity;
} VaultTable;

typedef struct {
    uint32_t checksum;
    uint32_t extracted_bytes;
    VaultTable table;
} VaultSession;

void vault_session_init(VaultSession *session);
void vault_session_free(VaultSession *session);
int vault_parse_archive(VaultSession *session, const uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif

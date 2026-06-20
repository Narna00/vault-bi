#ifndef VAULTBOX_INTERNAL_H
#define VAULTBOX_INTERNAL_H

#include "vault.h"

int vault_table_put(VaultTable *table, uint16_t id, uint8_t flags, const uint8_t *data, uint32_t size);
VaultEntry *vault_table_get(VaultTable *table, uint16_t id);
void vault_table_clear(VaultTable *table);
int vault_extract_entry(VaultSession *session, VaultEntry *entry, const uint8_t *script, uint32_t script_size);

#endif

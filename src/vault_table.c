#include "vault.h"

#include <stdlib.h>
#include <string.h>

static VaultEntry *vault_find_entry(VaultTable *table, uint16_t id) {
    for (size_t i = 0; i < table->count; ++i) {
        if (table->entries[i].id == id) {
            return &table->entries[i];
        }
    }
    return NULL;
}

static int vault_grow_table(VaultTable *table) {
    size_t next_capacity = table->capacity == 0 ? 4 : table->capacity * 2;
    VaultEntry *next = (VaultEntry *)realloc(table->entries, next_capacity * sizeof(VaultEntry));
    if (next == NULL) {
        return VAULT_ERR_NOMEM;
    }

    table->entries = next;
    table->capacity = next_capacity;
    return VAULT_OK;
}

int vault_table_put(VaultTable *table, uint16_t id, uint8_t flags, const uint8_t *data, uint32_t size) {
    VaultEntry *entry = vault_find_entry(table, id);
    if (entry == NULL) {
        if (table->count == table->capacity && vault_grow_table(table) != VAULT_OK) {
            return VAULT_ERR_NOMEM;
        }
        entry = &table->entries[table->count++];
        memset(entry, 0, sizeof(*entry));
        entry->id = id;
    } else {
        free(entry->data);
        entry->data = NULL;
    }

    entry->data = (uint8_t *)malloc(size == 0 ? 1 : size);
    if (entry->data == NULL) {
        entry->size = 0;
        return VAULT_ERR_NOMEM;
    }

    if (size != 0) {
        memcpy(entry->data, data, size);
    }
    entry->flags = flags;
    entry->size = size;
    return VAULT_OK;
}

VaultEntry *vault_table_get(VaultTable *table, uint16_t id) {
    return vault_find_entry(table, id);
}

void vault_table_clear(VaultTable *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < table->count; ++i) {
        free(table->entries[i].data);
    }
    free(table->entries);
    table->entries = NULL;
    table->count = 0;
    table->capacity = 0;
}

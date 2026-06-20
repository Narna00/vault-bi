#include "vault_internal.h"

#include <stdint.h>
#include <string.h>

static uint32_t read_u32_le(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

int vault_extract_entry(VaultSession *session, VaultEntry *entry, const uint8_t *script, uint32_t script_size) {
    uint8_t scratch[64];
    uint32_t cursor = 0;

    memset(scratch, 0, sizeof(scratch));

    while (cursor < script_size) {
        uint8_t op = script[cursor++];

        if (op == 0x01) {
            if (cursor + 8 > script_size) {
                return VAULT_ERR_BOUNDS;
            }

            uint32_t offset = read_u32_le(script + cursor);
            uint32_t amount = read_u32_le(script + cursor + 4);
            cursor += 8;

            if (offset + amount > entry->size) {
                return VAULT_ERR_BOUNDS;
            }
            memcpy(scratch, entry->data + offset, amount);
            session->extracted_bytes += amount;
        } else if (op == 0x02) {
            if (cursor >= script_size) {
                return VAULT_ERR_BOUNDS;
            }
            uint8_t rounds = script[cursor++];
            for (uint8_t i = 0; i < rounds; ++i) {
                session->checksum = (session->checksum * 33u) ^ scratch[i % sizeof(scratch)];
            }
        } else if (op == 0xff) {
            return VAULT_OK;
        } else {
            return VAULT_ERR_FORMAT;
        }
    }

    return VAULT_OK;
}

#include "vault_internal.h"

#include <stdlib.h>
#include <string.h>

enum {
    REC_STORE = 0x10,
    REC_EXTRACT = 0x20,
    REC_ALIAS = 0x30
};

static uint16_t read_u16_le(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0]) | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32_le(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

void vault_session_init(VaultSession *session) {
    memset(session, 0, sizeof(*session));
    session->checksum = 5381u;
}

void vault_session_free(VaultSession *session) {
    vault_table_clear(&session->table);
    session->checksum = 0;
    session->extracted_bytes = 0;
}

static int parse_store(VaultSession *session, const uint8_t *body, uint32_t body_size) {
    if (body_size < 3) {
        return VAULT_ERR_FORMAT;
    }

    uint16_t id = read_u16_le(body);
    uint8_t flags = body[2];
    return vault_table_put(&session->table, id, flags, body + 3, body_size - 3);
}

static int parse_extract(VaultSession *session, const uint8_t *body, uint32_t body_size) {
    if (body_size < 2) {
        return VAULT_ERR_FORMAT;
    }

    uint16_t id = read_u16_le(body);
    VaultEntry *entry = vault_table_get(&session->table, id);
    if (entry == NULL) {
        return VAULT_ERR_FORMAT;
    }

    return vault_extract_entry(session, entry, body + 2, body_size - 2);
}

static int parse_alias(VaultSession *session, const uint8_t *body, uint32_t body_size) {
    if (body_size < 4) {
        return VAULT_ERR_FORMAT;
    }

    uint16_t from_id = read_u16_le(body);
    uint16_t to_id = read_u16_le(body + 2);
    VaultEntry *from = vault_table_get(&session->table, from_id);
    if (from == NULL) {
        return VAULT_ERR_FORMAT;
    }

    uint8_t alias_len = body_size > 4 ? body[4] : (uint8_t)from->size;
    return vault_table_put(&session->table, to_id, from->flags, from->data, alias_len);
}

int vault_parse_archive(VaultSession *session, const uint8_t *data, size_t size) {
    if (session == NULL || data == NULL || size < 8) {
        return VAULT_ERR_FORMAT;
    }

    if (memcmp(data, "VLT1", 4) != 0) {
        return VAULT_ERR_FORMAT;
    }

    uint16_t version = read_u16_le(data + 4);
    uint16_t record_count = read_u16_le(data + 6);
    if (version != 1 || record_count > 256) {
        return VAULT_ERR_FORMAT;
    }

    size_t cursor = 8;
    for (uint16_t i = 0; i < record_count; ++i) {
        if (cursor + 5 > size) {
            return VAULT_ERR_BOUNDS;
        }

        uint8_t type = data[cursor++];
        uint32_t body_size = read_u32_le(data + cursor);
        cursor += 4;

        if (cursor + body_size > size) {
            return VAULT_ERR_BOUNDS;
        }

        const uint8_t *body = data + cursor;
        int status;
        if (type == REC_STORE) {
            status = parse_store(session, body, body_size);
        } else if (type == REC_EXTRACT) {
            status = parse_extract(session, body, body_size);
        } else if (type == REC_ALIAS) {
            status = parse_alias(session, body, body_size);
        } else {
            status = VAULT_ERR_FORMAT;
        }

        if (status != VAULT_OK) {
            return status;
        }

        cursor += body_size;
    }

    return VAULT_OK;
}

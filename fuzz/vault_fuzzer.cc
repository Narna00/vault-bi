#include <stddef.h>
#include <stdint.h>

extern "C" {
#include "src/vault.h"
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    VaultSession session;
    vault_session_init(&session);
    (void)vault_parse_archive(&session, data, size);
    vault_session_free(&session);
    return 0;
}

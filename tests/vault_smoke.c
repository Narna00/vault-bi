#include "src/vault.h"

#include <stdint.h>
#include <stdio.h>

int main(void) {
    const uint8_t archive[] = {
        'V', 'L', 'T', '1',
        1, 0,
        1, 0,
        0x10,
        7, 0, 0, 0,
        1, 0,
        0,
        'd', 'a', 't', 'a'
    };

    VaultSession session;
    vault_session_init(&session);
    int status = vault_parse_archive(&session, archive, sizeof(archive));
    vault_session_free(&session);

    if (status != VAULT_OK) {
        fprintf(stderr, "smoke parse failed: %d\n", status);
        return 1;
    }
    return 0;
}

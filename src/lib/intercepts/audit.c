#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"
#include "../util/templates/gen_audit.h"

AUDIT_HEADER()
AUDIT_CONSTRUCTOR(ossl_initialize)
AUDIT_LIB(libssl)
AUDIT_HOOKS {
    AUDIT_HOOK_SYMBOL(SSL_write)
    AUDIT_HOOK_SYMBOL(SSL_read)
    AUDIT_HOOK_SYMBOL(SSL_get_verify_result)
    AUDIT_HOOK_SYMBOL(SSL_CTX_set_verify)
    AUDIT_HOOK_DEFAULT()
}

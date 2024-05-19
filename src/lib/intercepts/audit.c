#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"
#include "../util/templates/gen_audit.h"

TEMPLATE_AUDIT_HEADER()
TEMPLATE_AUDIT_CONSTRUCTOR(ossl_initialize)
TEMPLATE_AUDIT_LIB(libssl)
TEMPLATE_AUDIT_SYMBOLS {
    TEMPLATE_AUDIT_SYMBOLS_ADD(hooked_SSL_write, SSL_write)
    TEMPLATE_AUDIT_SYMBOLS_ADD(hooked_SSL_read, SSL_read)
    TEMPLATE_AUDIT_SYMBOLS_ADD(hooked_SSL_get_verify_result, SSL_get_verify_result)
    TEMPLATE_AUDIT_SYMBOLS_ADD(hooked_SSL_CTX_set_verify, SSL_CTX_set_verify)
    TEMPLATE_AUDIT_SYMBOLS_DEFAULT()
}

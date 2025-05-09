#define _GNU_SOURCE

#include "../initializer.h"
#include "../payloads.h"
#include "../util/templates/gen_audit.h"

AUDIT_HEADER()
AUDIT_CONSTRUCTOR(ossl_initialize)
AUDIT_LIB(libssl)
AUDIT_PAYLOADS {
  AUDIT_PAYLOAD_SYMBOL(SSL_write)
  AUDIT_PAYLOAD_SYMBOL(SSL_read)
  AUDIT_PAYLOAD_SYMBOL(SSL_get_verify_result)
  AUDIT_PAYLOAD_SYMBOL(SSL_CTX_set_verify)
  AUDIT_PAYLOAD_DEFAULT()
}

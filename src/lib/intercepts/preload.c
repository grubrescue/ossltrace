#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"
#include "../util/templates/gen_preload.h"

TEMPLATE_PRELOAD_CONSTRUCTOR(ossl_initialize)

int 
SSL_write(SSL *ssl, const void *buf, int num) {
    TEMPLATE_PRELOAD_FUNC_BODY(hooked_SSL_write, SSL_write, ssl, buf, num)
}

int 
SSL_read(SSL *ssl, void *buf, int num) {
    TEMPLATE_PRELOAD_FUNC_BODY(hooked_SSL_read, SSL_read, ssl, buf, num)
}

long
SSL_get_verify_result(const SSL *ssl) {
    TEMPLATE_PRELOAD_FUNC_BODY(hooked_SSL_get_verify_result, SSL_get_verify_result, ssl)
}

void 
SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    TEMPLATE_PRELOAD_FUNC_BODY(hooked_SSL_CTX_set_verify, SSL_CTX_set_verify, ctx, mode, verify)
}

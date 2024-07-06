#define _GNU_SOURCE

#include "../payloads.h"
#include "../initializer.h"
#include "../util/templates/gen_preload.h"


PRELOAD_CONSTRUCTOR(ossl_initialize)


int 
SSL_write(SSL *ssl, const void *buf, int num) {
    PRELOAD_PAYLOAD_BODY(SSL_write, ssl, buf, num)
}


int 
SSL_read(SSL *ssl, void *buf, int num) {
    PRELOAD_PAYLOAD_BODY(SSL_read, ssl, buf, num)
}


long
SSL_get_verify_result(const SSL *ssl) {
    PRELOAD_PAYLOAD_BODY(SSL_get_verify_result, ssl)
}


void 
SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    PRELOAD_PAYLOAD_BODY(SSL_CTX_set_verify, ctx, mode, verify)
}

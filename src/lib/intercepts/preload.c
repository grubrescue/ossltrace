#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"

#include <stddef.h>
#include <dlfcn.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <gnu/libc-version.h>   
#include <gnu/lib-names.h>


__attribute__((constructor)) 
void
initialize_constructor() {
    ossl_initialize();
}


int 
SSL_write(SSL *ssl, const void *buf, int num) {
    if (GET_FUNC_PTR(orig_SSL_write) == NULL) {
        SET_FUNC_PTR(orig_SSL_write, dlsym(RTLD_NEXT, "SSL_write"));
    }

    hooked_SSL_write(ssl, buf, num);
}


int 
SSL_read(SSL *ssl, void *buf, int num) {
    if (GET_FUNC_PTR(orig_SSL_read) == NULL) {
        SET_FUNC_PTR(orig_SSL_read, dlsym(RTLD_NEXT, "SSL_read"));
    }

    hooked_SSL_read(ssl, buf, num);
}


long
SSL_get_verify_result(const SSL *ssl) {
    if (GET_FUNC_PTR(orig_SSL_get_verify_result) == NULL) {
        SET_FUNC_PTR(orig_SSL_get_verify_result, dlsym(RTLD_NEXT, "SSL_get_verify_result"));
    }

    hooked_SSL_get_verify_result(ssl);
}


void 
SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    if (GET_FUNC_PTR(orig_SSL_CTX_set_verify) == NULL) {
        SET_FUNC_PTR(orig_SSL_CTX_set_verify, dlsym(RTLD_NEXT, "SSL_CTX_set_verify"));
    }

    hooked_SSL_CTX_set_verify(ctx, mode, verify);
}

#pragma once

#include "../common.h"
#include <openssl/ssl.h>

// SSL_write
#define GEN_SYM_NAME SSL_write
#include "util/gen_callback_decls.h"

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num);


// SSL_read
#define GEN_SYM_NAME SSL_read
#include "util/gen_callback_decls.h"

int 
hooked_SSL_read(SSL *ssl, void *buf, int num);


// SSL_get_verify_result
#define GEN_SYM_NAME SSL_get_verify_result
#include "util/gen_callback_decls.h"

long 
hooked_SSL_get_verify_result(const SSL *ssl);


// SSL_set_verify
#define GEN_SYM_NAME SSL_CTX_set_verify
#include "util/gen_callback_decls.h"

void 
hooked_SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify);

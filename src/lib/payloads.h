#pragma once

#include <openssl/ssl.h>

#include "util/templates/gen_payload.h"

DECL_PAYLOAD(int, SSL_write, SSL *ssl, const void *buf, int num);
DECL_PAYLOAD(int, SSL_read, SSL *ssl, void *buf, int num);

void
init_ca_ignore_mode(char *ca_env);

DECL_PAYLOAD(long, SSL_get_verify_result, const SSL *ssl);
DECL_PAYLOAD(void, SSL_CTX_set_verify, SSL_CTX *ctx, int mode,
             SSL_verify_cb verify);

#pragma once

#include "../hooks.h"

int 
SSL_write(SSL *ssl, const void *buf, int num);

int 
SSL_read(SSL *ssl, void *buf, int num);

void 
SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify);

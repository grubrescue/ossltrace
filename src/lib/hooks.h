#pragma once

#include "../common.h"
#include <openssl/ssl.h>

// SSL_write
void 
set_SSL_write(void *symbol);

void *
get_SSL_write(void);

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num);


// SSL_read
void 
set_SSL_read(void *symbol);

void *
get_SSL_read(void);

int 
hooked_SSL_read(SSL *ssl, void *buf, int num);


// SSL_get_verify_result
void 
set_SSL_get_verify_result(void *symbol);

void *
get_SSL_get_verify_result(void);

long 
hooked_SSL_get_verify_result(const SSL *ssl);


// SSL_set_verify
void 
set_SSL_CTX_set_verify(void *symbol);

void *
get_SSL_CTX_set_verify(void);

void 
hooked_SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify);
#pragma once

#include "../common.h"
#include <openssl/ssl.h>

// SSL_write
void 
set_SSL_write_callback(void * symbol);

void *
get_SSL_write_callback(void);

int 
hooked_SSL_write(SSL * ssl, const void * buf, int num);

// SSL_read
void 
set_SSL_read_callback(void *symbol);

void *
get_SSL_read_callback(void);

int 
hooked_SSL_read(SSL * ssl, void * buf, int num);

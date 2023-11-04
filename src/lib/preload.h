#pragma once

#include "lib_common.h"
#include "hooked.h"

#define INPROC_SYMBOL_VISIBLE __attribute__((visibility("default")))

INPROC_SYMBOL_VISIBLE
int 
SSL_write(SSL *ssl, const void *buf, int num);

INPROC_SYMBOL_VISIBLE
int 
SSL_read(SSL *ssl, const void *buf, int num);
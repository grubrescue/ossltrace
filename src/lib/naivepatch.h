#pragma once

#include "hooks.h"

void *
dlopen(const char *filename, int flags);

int 
SSL_write(SSL *ssl, const void *buf, int num);

int 
SSL_read(SSL *ssl, void *buf, int num);

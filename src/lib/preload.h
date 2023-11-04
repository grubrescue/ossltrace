#pragma once

#include "hooked.h"

int 
SSL_write(SSL *ssl, const void *buf, int num);

int 
SSL_read(SSL *ssl, const void *buf, int num);

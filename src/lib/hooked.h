#pragma once

#define INPROC_SYMBOL_VISIBLE __attribute__((visibility("default")))
#define INPROC_OUTPUT_FILE_DIR_ENV_VAR "INPROC_OUTPUT_FILE_DIR"

#include <openssl/ssl.h>


// SSL_write
inline void 
set_SSL_write_callback(void *symbol);

inline void *
get_SSL_write_callback(void);

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num);


// SSL_read
inline void 
set_SSL_read_callback(void *symbol);

inline void *
get_SSL_read_callback(void);

int 
hooked_SSL_read(SSL *ssl, void *buf, int num);

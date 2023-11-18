#define _GNU_SOURCE

#include "preload.h"

#include <stddef.h>
#include <dlfcn.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <gnu/libc-version.h>   
#include <gnu/lib-names.h>

int 
SSL_write(SSL * ssl, const void * buf, int num) {
    if (get_SSL_write_callback() == NULL) {
        set_SSL_write_callback(dlsym(RTLD_NEXT, "SSL_write"));
    }

    hooked_SSL_write(ssl, buf, num);
}

int 
SSL_read(SSL * ssl, void * buf, int num) {
    if (get_SSL_read_callback() == NULL) {
        set_SSL_read_callback(dlsym(RTLD_NEXT, "SSL_read"));
    }

    hooked_SSL_read(ssl, buf, num);
}

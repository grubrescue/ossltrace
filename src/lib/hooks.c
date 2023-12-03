#define _GNU_SOURCE

#include "hooks.h"
#include "firewall.h"
#include "logger.h"

#include "util/vector.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <assert.h>
#include <gnu/libc-version.h>
#include <gnu/lib-names.h>
#include <fcntl.h>

// SSL_write

typedef int 
(*SSL_write_callback) (SSL *, const void *, int);

static SSL_write_callback original_SSL_write = NULL;

void 
set_SSL_write_callback(void *symbol) {
    assert(symbol != NULL);
    original_SSL_write = symbol;
}

void *
get_SSL_write_callback() {
    return original_SSL_write;
}

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num) {
    INPROC_LOG("\n*** SSL_write intercepted\nintermediate buffer size is %d, contents: \n\n", num)
    INPROC_LOG_BUF(buf, num)
    INPROC_LOG("\n")

    int retval;
    char *occurence = find_denylisted_words_occurence(buf, num);
    if (occurence != NULL) {
        INPROC_LOG("\n!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
        retval = -1;
    } else {
        retval = original_SSL_write(ssl, buf, num);
    }

    INPROC_LOG("return value is %d\n***\n\n", retval)
    return retval;
}


// SSL_read

typedef int 
(*SSL_read_callback) (SSL *, void *, int);

static SSL_read_callback original_SSL_read = NULL;

void 
set_SSL_read_callback(void *symbol) {
    assert(symbol != NULL);
    original_SSL_read = symbol;
}

void *
get_SSL_read_callback() {
    return original_SSL_read;
}

int 
hooked_SSL_read(SSL *ssl, void *buf, int num) {
    INPROC_LOG("\n*** SSL_read intercepted\n")

    int retval = original_SSL_read(ssl, buf, num);
    if (retval == -1) {
        INPROC_LOG("retval is -1, no buffer, sorry")
    } else {
        INPROC_LOG("intermediate buffer size is %d (num was %d) contents: \n\n", retval, num)
        INPROC_LOG_BUF(buf, retval)
        INPROC_LOG("\n");

        char *occurence = find_denylisted_words_occurence(buf, retval);

        if (occurence != NULL) {
            INPROC_LOG("!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
            retval = -1;
        }
    }

    INPROC_LOG("\n***\n")

    return retval;
}

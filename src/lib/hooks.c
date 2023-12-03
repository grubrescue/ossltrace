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
#define GEN_SYM_NAME SSL_write
#include "util/gen_callback_defs.h"

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num) {
    typedef int 
    (*SSL_write_callback) (SSL *, const void *, int);

    INPROC_LOG("\n*** SSL_write intercepted\nintermediate buffer size is %d, contents: \n\n", num)
    INPROC_LOG_BUF(buf, num)
    INPROC_LOG("\n")

    int retval;
    char *occurence = find_denylisted_words_occurence(buf, num);
    if (occurence != NULL) {
        INPROC_LOG("\n!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
        retval = -1;
    } else {
        retval = ((SSL_write_callback) original_SSL_write)(ssl, buf, num);
    }

    INPROC_LOG("return value is %d\n***\n\n", retval)
    return retval;
}


// SSL_read
#define GEN_SYM_NAME SSL_read
#include "util/gen_callback_defs.h"

int 
hooked_SSL_read(SSL *ssl, void *buf, int num) {
    typedef int 
    (*SSL_read_callback) (SSL *, void *, int);

    INPROC_LOG("\n*** SSL_read intercepted\n")

    int retval = ((SSL_read_callback) original_SSL_read)(ssl, buf, num);
    if (retval == -1) {
        INPROC_LOG("retval is -1, no buffer, sorry")
    } else {
        INPROC_LOG("intermediate buffer size is %d (num was %d) contents: \n\n", retval, num)
        INPROC_LOG_BUF(buf, retval)
        INPROC_LOG("\n")

        char *occurence = find_denylisted_words_occurence(buf, retval);

        if (occurence != NULL) {
            INPROC_LOG("!!! FOUND %s, PACKET REFUSED !!!\n", occurence)
            retval = -1;
        }
    }

    INPROC_LOG("\n***\n")

    return retval;
}


// SSL_get_verify_result
#define GEN_SYM_NAME SSL_get_verify_result
#include "util/gen_callback_defs.h"

long 
hooked_SSL_get_verify_result(const SSL *ssl) {
    typedef long 
    (*SSL_get_verify_result_callback) (const SSL *);

    INPROC_LOG("\n*** SSL_get_verify_result intercepted\n")

    long res = ((SSL_get_verify_result_callback) original_SSL_get_verify_result)(ssl);

    INPROC_LOG("retval is %ld, though we'll return 0", res)
    INPROC_LOG("\n***\n")

    return 0;
}

// SSL_CTX_set_verify
#define GEN_SYM_NAME SSL_CTX_set_verify
#include "util/gen_callback_defs.h"

void 
hooked_SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    typedef void 
    (*SSL_CTX_set_verify_callback) (SSL_CTX *, int, SSL_verify_cb);

    INPROC_LOG("\n*** SSL_CTX_set_verify intercepted\n")
    INPROC_LOG("setting SSL_VERIFY_NONE");

    ((SSL_CTX_set_verify_callback) original_SSL_CTX_set_verify)(ctx, SSL_VERIFY_NONE, NULL);

    INPROC_LOG("\n***\n")
}

#pragma once 

#define _GNU_SOURCE

#include "firewall-client.h"
#include "../common/logger.h"
#include "util/templates/gen_payload.h"

#include <openssl/ssl.h>

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


DEF_PAYLOAD(int, SSL_write, SSL *ssl, const void *buf, int num) {
    OSSLTRACE_LOG("\n*** SSL_write intercepted\nintermediate buffer size is %d, contents: \n\n", num)
    OSSLTRACE_LOG_BUF(buf, num)
    OSSLTRACE_LOG("\n")

    int retval;
    
    int refused = firewall_match_in_buf(buf, num);
    if (refused) {
        retval = -1;
    } else {
        retval = INVOKE_ORIGINAL(SSL_write, ssl, buf, num);
    }

    OSSLTRACE_LOG("return value is %d\n***\n\n", retval)
    return retval;
}


DEF_PAYLOAD(int, SSL_read, SSL *ssl, void *buf, int num) {
    OSSLTRACE_LOG("\n*** SSL_read intercepted\n")

    int retval = INVOKE_ORIGINAL(SSL_read, ssl, buf, num);
    if (retval == -1) {
        OSSLTRACE_LOG("retval is -1, no buffer, sorry")
    } else {
        OSSLTRACE_LOG("intermediate buffer size is %d (num was %d) contents: \n\n", retval, num)
        OSSLTRACE_LOG_BUF(buf, retval)
        OSSLTRACE_LOG("\n")

        int refused = firewall_match_in_buf(buf, retval);
        if (refused) {
            retval = -1;
        }
    }

    OSSLTRACE_LOG("\n***\n")

    return retval;
}


static volatile int is_ca_ignored = -1;

static void
init_ca_ignore_mode(char *ca_env) {
    is_ca_ignored = ca_env != NULL ? 1 : 0;
}

DEF_PAYLOAD(long, SSL_get_verify_result, const SSL *ssl) {
    OSSLTRACE_LOG("\n*** SSL_get_verify_result intercepted\n")

    long res = INVOKE_ORIGINAL(SSL_get_verify_result, ssl);
    if (is_ca_ignored) {
        OSSLTRACE_LOG("retval is %ld, though we'll return 0\n***\n", res)
        return 0;    
    } else {
        OSSLTRACE_LOG("retval is %ld, thus returning %ld\n***\n", res, res)
        return res;
    }
}


DEF_PAYLOAD(void, SSL_CTX_set_verify, SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    OSSLTRACE_LOG("\n*** SSL_CTX_set_verify intercepted\n")
    if (is_ca_ignored) {
        OSSLTRACE_LOG("setting SSL_VERIFY_NONE");
        INVOKE_ORIGINAL(SSL_CTX_set_verify, ctx, SSL_VERIFY_NONE, NULL);
    } else {
        OSSLTRACE_LOG("calling with no changes")
        INVOKE_ORIGINAL(SSL_CTX_set_verify, ctx, mode, verify);
    }

    OSSLTRACE_LOG("\n***\n")
}

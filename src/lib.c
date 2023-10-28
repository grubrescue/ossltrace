#include "lib.h"

#include <stddef.h>
#include <openssl/ssl.h>
#include <dlfcn.h>
#include <stdint.h>
#include <unistd.h>

#include <gnu/lib-names.h>

// typedef void * (*dlsym_callback)(void *restrict handle, const char *restrict symbol);
// dlsym_callback __real_dlsym = NULL;
// void * __dlsym_handle = NULL;


// INPROC_SO_EXPORT
// void *
// dlsym(void *restrict handle, const char *restrict symbol) {
//     if (__real_dlsym == NULL) {
//         __dlsym_handle = dlopen(LIBC_SO, RTLD_LAZY);
//         assert(__dlsym_handle != NULL);
//         __real_dlsym = dlsym(__dlsym_handle, "dlsym");
//         assert(__real_dlsym != NULL);
//     }
//     fprintf(stderr, "%s is gonna be found\n", symbol);
//     __real_dlsym(handle, symbol);
// }


typedef int (*SSL_write_callback)(SSL *ssl, const void *buf, int num);
SSL_write_callback __real_SSL_write = NULL;

INPROC_SO_EXPORT 
int 
SSL_write(SSL *ssl, const void *buf, int num) {
    if (__real_SSL_write == NULL) {
        __real_SSL_write = dlsym(RTLD_NEXT, "SSL_write");
    }

    // write(2, buf, num);
    // fprintf(stderr, "bufsize %d\n\n", num);
    
    // const uint8_t * charbuf = (const uint8_t*) buf;
    // for (int i = 0; i < num; i++) {
    //     fprintf(stderr, "%2.2x %c\n", ((const uint8_t*) buf)[i], ((const uint8_t*) buf)[i]);
    // }
    // fprintf(stderr, "\ndone now\n\n\n");
    

    return __real_SSL_write(ssl, buf, num);
}



typedef int (*SSL_read_callback)(SSL *ssl, void *buf, int num);
SSL_read_callback __real_SSL_read = NULL;

INPROC_SO_EXPORT 
int 
SSL_read(SSL *ssl, void *buf, int num) {
    if (__real_SSL_read == NULL) {
        __real_SSL_read = dlsym(RTLD_NEXT, "SSL_read");
    }

    int retval = __real_SSL_read(ssl, buf, num);
    fprintf(stderr, "red bytes %d\n\n", retval);

    if (retval > 0) {
        write(2, buf, retval);
    }
    fprintf(stderr, "\n\n");
    
    // const uint8_t * charbuf = (const uint8_t*) buf;
    // for (int i = 0; i < num; i++) {
    //     fprintf(stderr, "%2.2x %c\n", ((const uint8_t*) buf)[i], ((const uint8_t*) buf)[i]);
    // }
    // fprintf(stderr, "\ndone now\n\n\n");

    return retval;
}

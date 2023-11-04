#include "hooked.h"

#include <stddef.h>
#include <openssl/ssl.h>
#include <dlfcn.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <gnu/libc-version.h>   
#include <gnu/lib-names.h>
#include <fcntl.h>

// logging

static int log_fd = -1;
static FILE* log_file = NULL;
static int hooked_initialized = 0;

static void 
hooked_init() {
    char *output_file_path = getenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR);

    fprintf(stderr, "> %s\n\n", output_file_path);

    if (output_file_path == NULL) {
        log_fd = 1;
        log_file = stdout;
    } 
    
    else {
        log_fd = open(output_file_path, O_RDWR | O_CREAT, 00660);
        log_file = fdopen(log_fd, "w+"); // todo убрать принтфы, либо делать flush

        if (log_fd == -1 || log_file == NULL) {
            perror(output_file_path);
            fprintf(stderr, "log will be output to stdout\n\n");

            log_fd = 1;
            log_file = stdout;
        } 
    }

    hooked_initialized = 1;
}


// SSL_write

typedef int 
(*SSL_write_callback)
(SSL *ssl, const void *buf, int num);

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
    if (!hooked_initialized) {
        hooked_init();
    }

    write(2, "aboba", 6);


    fprintf(log_file, "\n\n --- SSL_write intercepted --- \n");
    fprintf(log_file, "intermediate buffer size is %d, contents: \n\n", num); 
    write(log_fd, buf, num);
    fprintf(log_file, "\n\n");
    
    int retval = original_SSL_write(ssl, buf, num);

    fprintf(log_file, "return value is %d\n ----------------------------- \n\n", retval);
    return retval;
}


// SSL_read

typedef int 
(*SSL_read_callback)
(SSL *ssl, void *buf, int num);

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
    if (!hooked_initialized) {
        hooked_init();
    }

    fprintf(log_file, "\n\n --- SSL_read intercepted --- \n");

    int retval = original_SSL_read(ssl, buf, num);
    if (retval == -1) {
        fprintf(log_file, "retval is -1, no buffer, sorry");
    } else {
        fprintf(log_file, "intermediate buffer size is %d (num was %d) contents: \n\n", retval, num);
        write(log_fd, buf, num);

    }

    fprintf(log_file, "\n\n ---------------------------- \n\n");

    return retval;
}

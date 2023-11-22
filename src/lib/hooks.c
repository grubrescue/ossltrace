#define _GNU_SOURCE

#include "hooks.h"

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

// logger
static int log_fd = 1;
static FILE *log_file = NULL;

static void
init_log() {
    char *output_file_path = getenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR);

    if (output_file_path == NULL) {
        log_fd = 1;
        log_file = stdout;
    } 
    
    else {
        log_fd = open(output_file_path, O_RDWR | O_CREAT, 00660);
        if (log_fd != -1) {
            log_file = fdopen(log_fd, "w+");
        }

        if (log_fd == -1 || log_file == NULL) {
            perror(output_file_path);
            fprintf(stderr, "log will be output to stdout\n\n"); fflush(stderr);

            log_fd = 1;
            log_file = stdout;
        } 
    }
}

#define INPROC_LOG(...) \
    fprintf(log_file, __VA_ARGS__); \
    fflush(log_file);

#define INPROC_LOG_BUF(buf, num) \
    write(log_fd, buf, num);


// firewall (filter)
static vector blacklist_words;
char buf[INPROC_MAX_BLACKLIST_WORD_LEN] = {};

static void 
print_string(const void *val) {
    INPROC_LOG("%s\n", val);
}

inline static void
init_firewall() {
    vector_init(&blacklist_words);

    char *blacklist_file_path = getenv(INPROC_BLACKLIST_FILE_ENV_VAR);
    if (blacklist_file_path == NULL) {
        return;
    } 

    FILE *blacklist_file = fopen(blacklist_file_path, "r");
    if (blacklist_file == NULL) {
        perror("fopen");
    }

    char *res;
    while ((res = fgets(buf, INPROC_MAX_BLACKLIST_WORD_LEN, blacklist_file)) != NULL) {    
        res = strdup(res);
        if (res != NULL) {

            int last_idx = strlen(res) - 1;
            if (res[last_idx] == '\n') {
                res[last_idx] = 0;    
            }
            vector_push(&blacklist_words, res);
        } else {
            break;
        }         
    }

    if (fclose(blacklist_file)) {                                                         
        perror("fclose");
    }

    INPROC_LOG("\n---\n! FORBIDDEN WORDS: \n");
    vector_foreach(&blacklist_words, print_string);
    INPROC_LOG("---\n");
}

static const void *firewall_buf;
static int firewall_num;
static int is_buf_contains(const void *needle) {
    return memmem(firewall_buf, firewall_num, needle, strlen(needle)) != NULL ? 1 : 0;
}

char *firewall(const void *buf, int num) {
    firewall_buf = buf;
    firewall_num = num;
    return (char *) vector_findfirst(&blacklist_words, is_buf_contains);
}

// main initializer
static int initialized = 0;

static void 
init() {
    init_log();
    init_firewall();

    initialized = 1;
}

// hooks themselves!

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
    if (!initialized) {
        init();
    }

    INPROC_LOG("\n--- SSL_write intercepted ---\nintermediate buffer size is %d, contents: \n\n", num)
    INPROC_LOG_BUF(buf, num)
    INPROC_LOG("\n")

    int retval;
    char *occurence = firewall(buf, num);
    if (occurence != NULL) {
        INPROC_LOG("\n!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
        retval = -1;
    } else {
        retval = original_SSL_write(ssl, buf, num);
    }

    INPROC_LOG("return value is %d\n-------------------------\n\n", retval)
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
    if (!initialized) {
        init();
    }

    INPROC_LOG("\n--- SSL_read intercepted ---\n")

    int retval = original_SSL_read(ssl, buf, num);
    if (retval == -1) {
        INPROC_LOG("retval is -1, no buffer, sorry")
    } else {
        INPROC_LOG("intermediate buffer size is %d (num was %d) contents: \n\n", retval, num)
        INPROC_LOG_BUF(buf, retval)
        INPROC_LOG("\n");

        char *occurence = firewall(buf, retval);

        if (occurence != NULL) {
            INPROC_LOG("\n!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
            retval = -1;
        }
    }

    INPROC_LOG("\n----------------------------\n")

    return retval;
}

#undef INPROC_LOG
#undef INPROC_LOG_BUF

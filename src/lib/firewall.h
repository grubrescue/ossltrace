#pragma once

#define _GNU_SOURCE

#include "util/vector.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048

static vector denylist_strings;
static volatile int initialized = 0;

static void 
print_string(const void *val) {
    OSSLTRACE_LOG("%s\n", val)
}

static void
init_firewall() {
    if (initialized) {
        OSSLTRACE_LOG("something strange: firewaller initialized twice\n");
        return;
    }

    vector_init(&denylist_strings);

    char *denylist_file_path = getenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR);
    if (denylist_file_path == NULL) {
        OSSLTRACE_LOG("filter: filename not found in env vars; filter won't work\n");
        return;
    } 

    FILE *denylist_file = fopen(denylist_file_path, "r");
    if (denylist_file == NULL) {
        perror("fopen");
        return;
    }

    char buf[OSSLTRACE_MAX_DENYLIST_WORD_LEN] = {};
    char *line;
    while ((line = fgets(buf, OSSLTRACE_MAX_DENYLIST_WORD_LEN, denylist_file)) != NULL) {    
        line = strdup(line);
        if (line != NULL) {
            int last_idx = strlen(line) - 1;
            if (line[last_idx] == '\n') {
                line[last_idx] = 0;    
            }
            vector_push(&denylist_strings, line);
        } else {
            break;
        }         
    }

    if (fclose(denylist_file)) {                                                         
        perror("fclose");
    }

    OSSLTRACE_LOG("\n! ! ! FORBIDDEN STRINGS: \n")
    vector_foreach(&denylist_strings, print_string);
    OSSLTRACE_LOG("! ! !\n")
}

static void  *is_buf_contains_buf;
static size_t is_buf_contains_num;

int 
is_buf_contains(const void *needle) {
    return memmem(is_buf_contains_buf, is_buf_contains_num, needle, strlen((const char *) needle)) != NULL ? 1 : 0; // va_args?
}

char *
find_denylisted_strings_occurence(const void *buf, size_t num) {
    is_buf_contains_buf = buf;
    is_buf_contains_num = num;
    return (char *) vector_findfirst(&denylist_strings, is_buf_contains); 
}

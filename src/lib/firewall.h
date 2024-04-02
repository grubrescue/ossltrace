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
    char *res;
    while ((res = fgets(buf, OSSLTRACE_MAX_DENYLIST_WORD_LEN, denylist_file)) != NULL) {    
        res = strdup(res);
        if (res != NULL) {

            int last_idx = strlen(res) - 1;
            if (res[last_idx] == '\n') {
                res[last_idx] = 0;    
            }
            vector_push(&denylist_strings, res);
        } else {
            break;
        }         
    }

    if (fclose(denylist_file)) {                                                         
        perror("fclose");
    }

    void 
    print_string(const void *val) {
        OSSLTRACE_LOG("%s\n", val)
    }

    OSSLTRACE_LOG("\n! ! ! FORBIDDEN STRINGS: \n")
    vector_foreach(&denylist_strings, print_string);
    OSSLTRACE_LOG("! ! !\n")
}

char *
find_denylisted_strings_occurence(const void *buf, int num) {
    int 
    is_buf_contains(const void *needle) {
        return memmem(buf, num, needle, strlen((const char *) needle)) != NULL ? 1 : 0; // va_args?
    }

    return (char *) vector_findfirst(&denylist_strings, is_buf_contains); 
}

#pragma once

#define _GNU_SOURCE

#include "../common.h"
#include "util/vector.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

// #include <errno.h>

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048

static vector denylist_words;

static void // todo корректная работа
init_firewall() {
    static int firewall_initialized = 0;
    if (firewall_initialized) {
        return;
    }

    firewall_initialized = 1;

    vector_init(&denylist_words);

    char *denylist_file_path = getenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR);
    if (denylist_file_path == NULL) {
        OSSLTRACE_LOG("%s", "filename not found in env vars; filter won't work");
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
            vector_push(&denylist_words, res);
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

    OSSLTRACE_LOG("\n! ! ! FORBIDDEN WORDS: \n")
    vector_foreach(&denylist_words, print_string);
    OSSLTRACE_LOG("! ! !\n")

    firewall_initialized = 1;
}

char *
find_denylisted_words_occurence(const void *buf, int num) {
    init_firewall();

    int 
    is_buf_contains(const void *needle) {
        return memmem(buf, num, needle, strlen((const char *) needle)) != NULL ? 1 : 0; // va_args?
    }

    return (char *) vector_findfirst(&denylist_words, is_buf_contains); 
}

#pragma once

#define _GNU_SOURCE

#include "../common.h"
#include "util/vector.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define INPROC_MAX_DENYLIST_WORD_LEN 2048

static vector denylist_words;

inline static void
init_firewall() {
    static int firewall_initialized = 0;
    if (firewall_initialized) {
        return;
    }

    vector_init(&denylist_words);

    char *denylist_file_path = getenv(INPROC_DENYLIST_FILE_ENV_VAR);
    if (denylist_file_path == NULL) {
        return;
    } 

    FILE *denylist_file = fopen(denylist_file_path, "r");
    if (denylist_file == NULL) {
        perror("fopen");
    }

    char buf[INPROC_MAX_DENYLIST_WORD_LEN] = {};
    char *res;
    while ((res = fgets(buf, INPROC_MAX_DENYLIST_WORD_LEN, denylist_file)) != NULL) {    
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

    // void 
    // print_string(const void *val) {
    //     INPROC_LOG("%s\n", val)
    // }

    // INPROC_LOG("\n! ! ! FORBIDDEN WORDS: \n")
    // vector_foreach(&denylist_words, print_string);
    // INPROC_LOG("! ! !\n")
}

char *
find_denylisted_words_occurence(const void *buf, int num) {
    init_firewall();

    int 
    is_buf_contains(const void *needle) {
        return memmem(buf, num, needle, strlen((const char *) needle)) != NULL ? 1 : 0; // va_args?
    }

    return (char *) vector_findfirst(&denylist_words, is_buf_contains); // todo more adequate
}

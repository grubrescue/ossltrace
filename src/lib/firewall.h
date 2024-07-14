#pragma once

#define _GNU_SOURCE

#include "util/list.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048


static strlist *deny_strs = NULL;
static volatile int initialized = 0;


static void
init_firewall() {
    if (initialized) {
        OSSLTRACE_LOG("something strange: firewaller initialized twice\n");
        return;
    }

    deny_strs = strlist_create();

    char *denylist_file_path = getenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR);
    if (denylist_file_path == NULL) {
        OSSLTRACE_LOG("firewall: filename not found in " 
            OSSLTRACE_DENYLIST_FILE_ENV_VAR "; denylist is empty\n");
        return;
    } 

    FILE *denylist_file = fopen(denylist_file_path, "r");
    if (denylist_file == NULL) {
        perror("fopen");
        OSSLTRACE_LOG("firewall: couldn't initialize")
        return;
    }

    char buf[OSSLTRACE_MAX_DENYLIST_WORD_LEN];
    char *line;
    while ((line = fgets(buf, OSSLTRACE_MAX_DENYLIST_WORD_LEN, denylist_file)) != NULL) {    
        line = strdup(line);
        if (line != NULL) {
            int last_idx = strlen(line) - 1;
            if (line[last_idx] == '\n') {
                line[last_idx] = 0;    
            }
            strlist_add(deny_strs, line);
        } else {
            perror("strdup");
            exit(EXIT_FAILURE);
        }         
    }

    if (fclose(denylist_file)) {                                                         
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    OSSLTRACE_LOG("\n! ! ! FORBIDDEN STRINGS: \n%s\n", strlist_repr(deny_strs, '\n'));
}


// Public.
char *
firewall_match_in_buf(const void *buf, size_t buf_n) {
    return strlist_find_any_in_buf(deny_strs, buf, buf_n);
}

char *
firewall_get_all_strings() {
    return strlist_repr(deny_strs, '\n');
}

void
firewall_add_str(char *str) {
    strlist_add(deny_strs, str);
}

int
firewall_remove_str(char *str) {
    return strlist_remove(deny_strs, str);
}

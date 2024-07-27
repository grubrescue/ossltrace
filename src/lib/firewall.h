#pragma once

#define _GNU_SOURCE

#include "util/strlist.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048

static strlist *deny_strs = NULL;
static pthread_mutex_t strlist_mutex;
static volatile int initialized = 0;

static void
init_firewall() {
    if (initialized) {
        OSSLTRACE_LOG("something strange: firewaller initialized twice\n");
        return;
    }

    int retval = pthread_mutex_init(&strlist_mutex, NULL);
    deny_strs = strlist_create();

    if (retval != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    initialized = 1;

    char *denylist_file_path = getenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR);
    if (denylist_file_path == NULL) {
        OSSLTRACE_LOG("!!! firewall: filename not found in " 
            OSSLTRACE_DENYLIST_FILE_ENV_VAR "; assuming denylist is empty.\n");
        return;
    } 

    FILE *denylist_file = fopen(denylist_file_path, "r");
    if (denylist_file == NULL) {
        perror("fopen");
        OSSLTRACE_LOG("firewall: couldn't initialize")
        return;
    }
    
    pthread_mutex_lock(&strlist_mutex);

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

    pthread_mutex_unlock(&strlist_mutex);

    if (fclose(denylist_file)) {                                                         
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    OSSLTRACE_LOG("\n!!! Firewall initialized. Forbidden strings:\n%s\n", strlist_repr(deny_strs, '\n'));
}


// Public.

const char *
firewall_match_in_buf(const void *buf, size_t buf_n) {
    pthread_mutex_lock(&strlist_mutex);
    OSSLTRACE_LOG("\n!!! Blocking list:\n%s\n", strlist_repr(deny_strs, '\n'));
    OSSLTRACE_LOG("getpid()=%d\n\n", getpid());
    const char *matched_str = strlist_find_any_in_buf(deny_strs, buf, buf_n);
    OSSLTRACE_LOG("\nRES : %s\n", matched_str);
    pthread_mutex_unlock(&strlist_mutex);
    return matched_str;
}


const char *
firewall_get_all_strings() {
    pthread_mutex_lock(&strlist_mutex);
    const char *repr = strlist_repr(deny_strs, '\n');
    OSSLTRACE_LOG("strings list request\n");
    OSSLTRACE_LOG("\n!!! Blocking list:\n%s\n", strlist_repr(deny_strs, '\n'));
    OSSLTRACE_LOG("getpid()=%d\n\n", getpid());
    pthread_mutex_unlock(&strlist_mutex); // todo free
    return repr;
}


void
firewall_add_str(const char *str) {
    pthread_mutex_lock(&strlist_mutex);
    strlist_add(deny_strs, str);
    OSSLTRACE_LOG("string added: %s\n", str);
    OSSLTRACE_LOG("\n!!! Blocking list:\n%s\n", strlist_repr(deny_strs, '\n'));
    OSSLTRACE_LOG("getpid()=%d\n\n", getpid());
    pthread_mutex_unlock(&strlist_mutex);
}


int
firewall_remove_str(const char *str) {
    pthread_mutex_lock(&strlist_mutex);
    int deleted_count = strlist_remove(deny_strs, str);
    if (deleted_count) {
        OSSLTRACE_LOG("removed %s\n", str);
    } else {
        OSSLTRACE_LOG("hasn't found, so hasn't removed %s\n", str);
    }
    pthread_mutex_unlock(&strlist_mutex);
    return deleted_count;
}

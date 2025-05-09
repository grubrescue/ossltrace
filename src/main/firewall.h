#pragma once

#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/logger.h"
#include "util/strlist.h"

#define OSSLTRACE_MAX_DENYLIST_WORD_LEN 2048

static strlist *deny_strs = NULL;
static pthread_mutex_t strlist_mutex;
static volatile int initialized = 0;

static void
init_firewall(char *denylist_file_path) {
  assert(!initialized);

  int retval = pthread_mutex_init(&strlist_mutex, NULL);
  deny_strs = strlist_create();

  if (retval != 0) {
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }

  initialized = 1;

  if (denylist_file_path == NULL) {
    OSSLTRACE_LOG("*** firewall: assuming denylist is empty.\n");
    return;
  }

  FILE *denylist_file = fopen(denylist_file_path, "r");
  if (denylist_file == NULL) {
    perror("fopen");
    OSSLTRACE_LOG("*** firewall: couldn't initialize\n")
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

  OSSLTRACE_LOG("\n*** firewall initialized; forbidden strings list:\n%s\n", strlist_repr(deny_strs, '\n'));
}

// Public.

const char *
firewall_get_all_strings() {
  pthread_mutex_lock(&strlist_mutex);
  const char *repr = strlist_repr(deny_strs, '\n');
  OSSLTRACE_LOG("*** strings list request\n");
  pthread_mutex_unlock(&strlist_mutex);  // todo rwlock
  return repr;
}

void
firewall_add_str(const char *str) {
  pthread_mutex_lock(&strlist_mutex);
  strlist_add(deny_strs, str);
  OSSLTRACE_LOG("*** string added: %s\n", str);
  pthread_mutex_unlock(&strlist_mutex);
}

int
firewall_remove_str(const char *str) {
  pthread_mutex_lock(&strlist_mutex);
  int deleted_count = strlist_remove(deny_strs, str);
  if (deleted_count) {
    OSSLTRACE_LOG("*** removed %s\n", str);
  } else {
    OSSLTRACE_LOG("*** hasn't found, so hasn't removed %s\n", str);
  }
  pthread_mutex_unlock(&strlist_mutex);
  return deleted_count;
}

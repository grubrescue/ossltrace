#include "firewall.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/logger.h"
#include "util/strlist.h"

static strlist *deny_strs = NULL;
static pthread_mutex_t strlist_mutex;
static int firewall_initialized = 0;

void
init_firewall(char *denylist_file_path) {
  assert(!firewall_initialized);

  int retval = pthread_mutex_init(&strlist_mutex, NULL);
  deny_strs = strlist_create();

  if (retval != 0) {
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }

  firewall_initialized = 1;

  if (denylist_file_path == NULL) {
    OSSLTRACE_LOG("~ firewall: assuming denylist is empty.\n");
    return;
  }

  FILE *denylist_file = fopen(denylist_file_path, "r");
  if (denylist_file == NULL) {
    perror("fopen");
    OSSLTRACE_LOG("~ firewall: couldn't initialize\n")
    return;
  }

  pthread_mutex_lock(&strlist_mutex);

  char buf[OSSLTRACE_MAX_DENYLIST_WORD_LEN];
  char *line;
  while ((line = fgets(buf, OSSLTRACE_MAX_DENYLIST_WORD_LEN, denylist_file)) !=
         NULL) {
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

  OSSLTRACE_LOG("\n~ firewall: forbidden strings list:\n%s\n",
                strlist_repr(deny_strs, '\n'));
}

char *
firewall_get_all_strings() {
  pthread_mutex_lock(&strlist_mutex);
  char *repr = strlist_repr(deny_strs, '\n');
  OSSLTRACE_LOG("~ firewall: strings list request\n");
  pthread_mutex_unlock(&strlist_mutex);
  return repr;
}

void
firewall_add_str(const char *str) {
  pthread_mutex_lock(&strlist_mutex);
  strlist_add(deny_strs, str);
  OSSLTRACE_LOG("~ firewall: string added: %s\n", str);
  pthread_mutex_unlock(&strlist_mutex);
}

int
firewall_remove_str(const char *str) {
  pthread_mutex_lock(&strlist_mutex);
  int deleted_count = strlist_remove(deny_strs, str);
  if (deleted_count) {
    OSSLTRACE_LOG("~ firewall: removed %s\n", str);
  } else {
    OSSLTRACE_LOG("~ firewall: hasn't found, so hasn't removed %s\n", str);
  }
  pthread_mutex_unlock(&strlist_mutex);
  return deleted_count;
}

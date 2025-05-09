#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
init_log(char *output_file_path);

extern int log_fd;
extern FILE *log_file;

#define OSSLTRACE_LOG(...)        \
  assert(log_file != NULL);       \
  fprintf(log_file, __VA_ARGS__); \
  fflush(log_file);

#define OSSLTRACE_LOG_BUF(buf, num) \
  assert(log_file != NULL);         \
  write(log_fd, buf, num);

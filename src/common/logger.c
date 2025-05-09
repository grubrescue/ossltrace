#include "logger.h"

#include <fcntl.h>

#include "envvars.h"

int log_fd = -1;
FILE *log_file = NULL;

void
init_log(char *output_file_path) {
  if (log_fd != -1) {
    fprintf(stderr, "something strange: logger initialized twice\n");
    return;
  }

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
      fprintf(stderr, "!! log will be output to stdout\n\n");
      fflush(stderr);

      log_fd = 1;
      log_file = stdout;
    }
  }
}

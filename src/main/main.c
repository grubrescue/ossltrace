#include "main.h"

#include <argp.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../common/logger.h"
#include "server.h"

typedef struct {
  enum { PRELOAD,
         AUDIT,
         CAPSTONE } mode;
  int ignore_ca;
  char *output_file_path;
  char *denylist_file_path;
  char **child_argv;
} arguments;

void
print_usage(FILE *where, const char *pathname) {
  fprintf(where, "usage: %s [options] command [command args]\n", pathname);
  fprintf(where, "see list of options by typing %s -h/--help\n", pathname);
}

void
print_help(FILE *where, const char *pathname) {
  print_usage(where, pathname);
  fprintf(stdout, "no help yet, sorry!\n\n");
}

int
main(int argc, char **argv) {
  arguments arguments;
  arguments.output_file_path = NULL;
  arguments.child_argv = NULL;
  arguments.denylist_file_path = NULL;
  arguments.mode = PRELOAD;
  arguments.ignore_ca = 0;

  int finished_parsing = 0;
  while (!finished_parsing) {
    int option_index = 0;

    static struct option long_options[] = {
        {"mode", required_argument, NULL, 'm'},
        {"output", required_argument, NULL, 'o'},
        {"quiet", no_argument, NULL, 'q'},
        {"denylist", required_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {"ignore-ca", no_argument, NULL, 'i'},
        {NULL, 0, NULL, 0}};

    int c = getopt_long(argc, argv, "+m:o:qd:hi", long_options, &option_index);

    switch (c) {
      case 'm':
        if (!strcmp(optarg, "preload")) {
          arguments.mode = PRELOAD;
        } else if (!strcmp(optarg, "audit")) {
          arguments.mode = AUDIT;
        } else if (!strcmp(optarg, "capstone")) {
          arguments.mode = CAPSTONE;
        } else {
          print_usage(stderr, argv[0]);
          exit(EXIT_FAILURE);
        }
        break;
      case 'o':
        arguments.output_file_path = strdup(optarg);
        break;
      case 'q':
        arguments.output_file_path = strdup("/dev/null");
        break;
      case 'd':
        arguments.denylist_file_path = strdup(optarg);
        break;
      case 'h':
        print_help(stdout, argv[0]);
        exit(EXIT_SUCCESS);
        break;
      case 'i':
        arguments.ignore_ca = 1;
        break;
      case '?':
        print_usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
        break;
      default:
        finished_parsing = 1;
        break;
    }
  }

  if (optind < argc) {
    arguments.child_argv = &argv[optind];
  } else {
    print_usage(stderr, argv[0]);
    exit(EXIT_FAILURE);
  }

  // Arguments to transmit to child.

  if (arguments.output_file_path != NULL) {
    setenv(OSSLTRACE_LOG_OUTPUT_FILE_ENV_VAR, arguments.output_file_path, 1);
    free(arguments.output_file_path);
  } else {
    unsetenv(OSSLTRACE_LOG_OUTPUT_FILE_ENV_VAR);
  }

  if (arguments.ignore_ca) {
    setenv(OSSLTRACE_IGNORE_CA_ENV_VAR, "1", 1);
  } else {
    unsetenv(OSSLTRACE_IGNORE_CA_ENV_VAR);
  }

  switch (arguments.mode) {
    case PRELOAD:
      char *preload_lib_path = getenv(OSSLTRACE_PRELOAD_ENV_VAR);
      if (preload_lib_path == NULL) {
        preload_lib_path = OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH;
      }

      if (access(preload_lib_path, X_OK) == -1) {
        perror(preload_lib_path);
        exit(EXIT_FAILURE);
      }

      setenv("LD_PRELOAD", preload_lib_path, 1);
      break;

    case AUDIT:
      char *audit_lib_path = getenv(OSSLTRACE_AUDIT_ENV_VAR);
      if (audit_lib_path == NULL) {
        audit_lib_path = OSSLTRACE_DEFAULT_AUDIT_LIB_PATH;
      }

      if (access(audit_lib_path, X_OK) == -1) {
        perror(audit_lib_path);
        exit(EXIT_FAILURE);
      }

      setenv("LD_AUDIT", audit_lib_path, 1);
      break;

    case CAPSTONE:
      char *capstone_lib_path = getenv(OSSLTRACE_CAPSTONE_ENV_VAR);
      if (capstone_lib_path == NULL) {
        capstone_lib_path = OSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH;

#ifndef OSSLTRACE_CAPSTONE_AVAILABLE
        fprintf(stderr,
                "WARNING: As there was no Capstone instance "
                "available during installation, you may have "
                "to specify the path to Capstone-based ossltrace library "
                "explicitly.\nIf error occurs, try setting the " OSSLTRACE_CAPSTONE_ENV_VAR
                " environment variable. ");
#endif
      }

      if (access(capstone_lib_path, X_OK) == -1) {
        perror(capstone_lib_path);
        exit(EXIT_FAILURE);
      }

      setenv("LD_PRELOAD", capstone_lib_path, 1);
      break;

    default:
      assert(0);
  }

  char *socket_path = malloc(strlen(SOCKET_PATH_PREFIX) + 12);  // should be enough
  snprintf(socket_path, strlen(SOCKET_PATH_PREFIX) + 12, "%s%d", SOCKET_PATH_PREFIX, getpid());
  setenv(OSSLTRACE_SOCKET_PATH_ENV_VAR, socket_path, 1);

  pid_t pid = fork();
  if (pid == 0 /*child*/) {
    if (execvp(arguments.child_argv[0], arguments.child_argv) == -1) {
      perror(arguments.child_argv[0]);
      exit(EXIT_FAILURE);
    }
    assert(0 && "Unreachable");
  } else if (pid > 0 /*parent*/) {
    init_log(arguments.output_file_path);
    OSSLTRACE_LOG("*** Started subprocess with pid=%d\n", pid);

    init_firewall(arguments.denylist_file_path);

    run_parasite_server(socket_path);

    int status;
    wait(&status);
    printf("Return code: %d\n", WEXITSTATUS(status));

    OSSLTRACE_LOG("*** Subprocess exited with return code %d\n", pid);
    return status;
  }

  return EXIT_SUCCESS;
}

#include "main.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void print_usage(FILE *where, char *pathname) {
    fprintf(stderr, "usage: %s <command> <mode: preload|audit> [command args]\n", pathname);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "%s: not enough args\n", argv[0]);
        print_usage(stderr, argv[0]);

        exit(EXIT_FAILURE);
    }

    char *mode = argv[1];
    char *child_file = argv[2];
    char **child_argv = argv+2;

    if (strcmp(mode, "preload") == 0) {
        char *lib_path = getenv(INPROC_PRELOAD_ENV_VAR);
        if (lib_path == NULL) {
            lib_path = DEFAULT_INPROC_PRELOAD_LIB_PATH;
        }
        setenv("LD_PRELOAD", lib_path, 1);
    } else if (strcmp(mode, "audit") == 0) {
        char *lib_path = getenv(INPROC_AUDIT_ENV_VAR);
        if (lib_path == NULL) {
            lib_path = DEFAULT_INPROC_AUDIT_LIB_PATH;
        }
        setenv("LD_AUDIT", lib_path, 1);
    } else {
        fprintf(stderr, "%s: second argument must be mode. options: preload, audit\n", argv[0]);
        print_usage(stderr, argv[0]);

        exit(EXIT_FAILURE);
    }
 
    pid_t pid = fork();
    if (pid == 0 /*child*/) {
        if (execvp(child_file, child_argv) == -1) {
            perror(child_file);
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0 /*parent*/) {
        int status;
        wait(&status);
        printf("\n~ '%s' exited with return code %u ~ \n\n", child_file, WEXITSTATUS(status));
    } else {
        perror("unforkable");
    }
    
    return EXIT_SUCCESS;
}

#include "main.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void print_usage(char *pathname) {
    fprintf(stderr, "usage: %s <command> [command args]\n", pathname);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "sorry, not enough args\n");
        print_usage(argv[0]);

        exit(EXIT_FAILURE);
    }

    char *libpath = getenv("INPROC_LIB_PATH");
    if (libpath == NULL) {
        libpath = DEFAULT_INPROC_LIB_PATH;
    }

    char* child_file = argv[1];
    char** child_argv = argv+1;

    setenv("LD_PRELOAD", libpath, 1);

    if (execvp(child_file, child_argv) == -1) {
        perror(child_file);
        exit(EXIT_FAILURE);
    }
}

#include "main.h"

#include <argp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

struct arguments {
    enum { PRELOAD, AUDIT, NAIVEPATCH } mode;
    char *output_file_path;
    char *blacklist_file_path;
    char **child_argv;
};

void 
print_usage(FILE *where, char *pathname) {
    fprintf(where, "usage: %s [options] command [command args]\n", pathname);
    fprintf(where, "check list of options by typing %s -h/--help\n", pathname);
}

void 
print_help(FILE *where, char *pathname) {
    print_usage(where, pathname);
    fprintf(stdout, "no help yet, sorry!\n\n");
}

int 
main(int argc, char **argv) {
    struct arguments arguments;
    arguments.output_file_path = NULL; 
    arguments.child_argv = NULL; 
    arguments.blacklist_file_path = NULL;
    arguments.mode = PRELOAD;

    int finished_parsing = 0;
    while (!finished_parsing) {
        int option_index = 0;

        static struct option long_options[] = {
            { "method",    required_argument, NULL, 'm' },
            { "output",    required_argument, NULL, 'o' },
            { "quiet",     no_argument,       NULL, 'q' },
            { "blacklist", required_argument, NULL, 'b' },
            { "help",      no_argument,       NULL, 'h' },
            { NULL,        0,                 NULL,  0  }
        };

        int c = getopt_long(argc, argv, "+m:o:qb:h", long_options, &option_index);

        switch (c) {
            case 'm':
                if (!strcmp(optarg, "preload")) {
                    arguments.mode = PRELOAD;
                } else if (!strcmp(optarg, "audit")) {
                    arguments.mode = AUDIT;
                } else if (!strcmp(optarg, "naivepatch")) {
                    arguments.mode = NAIVEPATCH;                    
                } else {
                    print_usage(stderr, argv[0]);
                }
                break;
            case 'o':
                arguments.output_file_path = strdup(optarg);
                break;
            case 'q':
                arguments.output_file_path = strdup("/dev/null");
                break;
            case 'b':
                arguments.blacklist_file_path = strdup(optarg);
                break;
            case 'h':
                print_help(stdout, argv[0]);
                exit(EXIT_SUCCESS);
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

    if (arguments.output_file_path != NULL) {
        setenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR, arguments.output_file_path, 1);
        free(arguments.output_file_path);
    } else {
        unsetenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR);
    }

    if (arguments.blacklist_file_path != NULL) {
        setenv(INPROC_BLACKLIST_FILE_ENV_VAR, arguments.blacklist_file_path, 1);
        free(arguments.blacklist_file_path);
    } else {
        unsetenv(INPROC_BLACKLIST_FILE_ENV_VAR);
    }

    switch(arguments.mode) {
        case PRELOAD:
            char *preload_lib_path = getenv(INPROC_PRELOAD_ENV_VAR);
            if (preload_lib_path == NULL) {
                preload_lib_path = INPROC_DEFAULT_PRELOAD_LIB_PATH;
            }
            setenv("LD_PRELOAD", preload_lib_path, 1);
            break;

        case AUDIT:
            char *audit_lib_path = getenv(INPROC_AUDIT_ENV_VAR);
            if (audit_lib_path == NULL) {
                audit_lib_path = INPROC_DEFAULT_AUDIT_LIB_PATH;
            }
            setenv("LD_AUDIT", audit_lib_path, 1);
            break;
        case NAIVEPATCH:
            char *naivepatch_lib_path = getenv(INPROC_NAIVEPATCH_ENV_VAR);
            if (naivepatch_lib_path == NULL) {
                naivepatch_lib_path = INPROC_DEFAULT_NAIVEPATCH_LIB_PATH;
            }
            setenv("LD_PRELOAD", naivepatch_lib_path, 1);
        default:
    }

    if (execvp(arguments.child_argv[0], arguments.child_argv) == -1) {
        perror(arguments.child_argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

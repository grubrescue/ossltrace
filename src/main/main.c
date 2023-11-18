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
    enum { PRELOAD, AUDIT } mode;
    char *output_file_path;
    char **child_argv;
};

void 
print_usage(FILE *where, char *pathname) {
    fprintf(where, "usage: %s -m|--mode: preload|audit> [command args]\n", pathname);
}

int 
main(int argc, char **argv) {
    struct arguments arguments;
    arguments.output_file_path = NULL; 
    arguments.child_argv = NULL; // required
    arguments.mode = PRELOAD;

    int finishedParsing = 0;
    while (!finishedParsing) {
        int option_index = 0;

        static struct option long_options[] = {
            { "method", required_argument, NULL, 'm' },
            { "output", required_argument, NULL, 'o' },
            { "filter", required_argument, NULL, 'f' },
            { NULL,     0,                 NULL,  0  }
        };

        int c = getopt_long(argc, argv, "+m:o:f:", long_options, &option_index);

        switch (c) {
            case 'm':
                if (!strcmp(optarg, "preload")) {
                    arguments.mode = PRELOAD;
                } else if (!strcmp(optarg, "audit")) {
                    arguments.mode = AUDIT;
                } else {
                    print_usage(stderr, argv[0]);
                }
                break;
            case 'o':
                arguments.output_file_path = strdup(optarg);
                break;
            case 'f':
                printf("option f with value '%s'\n", optarg);
                break;
            case '?':
                print_usage(stderr, argv[0]);
                exit(EXIT_FAILURE);
            default:
                finishedParsing = 1;
        }   
    }

    if (optind < argc) {
        arguments.child_argv = &argv[optind];
    } else {
        print_usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0 /*child*/) {
        if (arguments.output_file_path != NULL) {
            setenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR, arguments.output_file_path, 1);
            free(arguments.output_file_path);
        } else {
            unsetenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR);
        }

        switch(arguments.mode) {
            case PRELOAD:
                char *preload_lib_path = getenv(INPROC_PRELOAD_ENV_VAR);
                if (preload_lib_path == NULL) {
                    preload_lib_path = DEFAULT_INPROC_PRELOAD_LIB_PATH;
                }
                setenv("LD_PRELOAD", preload_lib_path, 1);
                break;

            case AUDIT:
                char *audit_lib_path = getenv(INPROC_AUDIT_ENV_VAR);
                if (audit_lib_path == NULL) {
                    audit_lib_path = DEFAULT_INPROC_AUDIT_LIB_PATH;
                }
                setenv("LD_AUDIT", audit_lib_path, 1);
                break;
                
            default:
        }

        if (execvp(arguments.child_argv[0], arguments.child_argv) == -1) {
            perror(arguments.child_argv[0]);
            exit(EXIT_FAILURE);
        }
    } 
    
    else if (pid > 0 /*parent*/) {
        int status;
        wait(&status);
        printf("\n~ '%s' exited with return code %u ~\n\n", arguments.child_argv[0], WEXITSTATUS(status));
    } 
    
    else {
        perror("unforkable");
    }
    
    return EXIT_SUCCESS;
}

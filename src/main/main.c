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

struct Arguments {
    enum { PRELOAD, AUDIT, CAPSTONE } mode;
    int  ignore_ca;
    char *output_file_path;
    char *denylist_file_path;
    char **child_argv;
};


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
    struct Arguments arguments;
    arguments.output_file_path = NULL; 
    arguments.child_argv = NULL; 
    arguments.denylist_file_path = NULL;
    arguments.mode = PRELOAD;
    arguments.ignore_ca = 0;

    int finished_parsing = 0;
    while (!finished_parsing) {
        int option_index = 0;

        static struct option long_options[] = {
            { "mode",      required_argument, NULL, 'm' },
            { "output",    required_argument, NULL, 'o' },
            { "quiet",     no_argument,       NULL, 'q' },
            { "denylist",  required_argument, NULL, 'd' },
            { "help",      no_argument,       NULL, 'h' },
            { "ignore-ca", no_argument,       NULL, 'i' },
            { NULL,        0,                 NULL,  0  }
        };

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

    if (arguments.output_file_path != NULL) {
        setenv(OSSLTRACE_LOG_OUTPUT_FILE_ENV_VAR, arguments.output_file_path, 1);
        free(arguments.output_file_path);
    } else {
        unsetenv(OSSLTRACE_LOG_OUTPUT_FILE_ENV_VAR);
    }

    if (arguments.denylist_file_path != NULL) {
        int err = setenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR, arguments.denylist_file_path, 1);
        if (err) {
            perror("setenv");
            exit(EXIT_FAILURE);
        }
        free(arguments.denylist_file_path);
    } else {
        unsetenv(OSSLTRACE_DENYLIST_FILE_ENV_VAR);
    }

    if (arguments.ignore_ca) {
        setenv(OSSLTRACE_IGNORE_CA_ENV_VAR, "1", 1);
    } else {
        unsetenv(OSSLTRACE_IGNORE_CA_ENV_VAR);
    }

    switch(arguments.mode) {
        case PRELOAD:
            char *preload_lib_path = getenv(OSSLTRACE_PRELOAD_ENV_VAR);
            if (preload_lib_path == NULL) {
                preload_lib_path = OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH;
            }

            if (!access(preload_lib_path, X_OK)) {
                perror(preload_lib_path);
            }

            setenv("LD_PRELOAD", preload_lib_path, 1);
            break;

        case AUDIT:
            char *audit_lib_path = getenv(OSSLTRACE_AUDIT_ENV_VAR);
            if (audit_lib_path == NULL) {
                audit_lib_path = OSSLTRACE_DEFAULT_AUDIT_LIB_PATH;
            }

            if (!access(audit_lib_path, X_OK)) {
                perror(audit_lib_path);
            }

            setenv("LD_AUDIT", audit_lib_path, 1);
            break;

        case CAPSTONE:
            char *capstone_lib_path = getenv(OSSLTRACE_CAPSTONE_ENV_VAR);
            if (capstone_lib_path == NULL) {
#ifdef OSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH
                capstone_lib_path = OSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH;
#else
                fprintf(stderr, "As there was no Capstone instance available during installation, "
                    "you have to specify the path to Capstone library explicitly"
                    "using "OSSLTRACE_CAPSTONE_ENV_VAR" environment variable. Exiting now...");
                return EXIT_FAILURE;
#endif
            }

            if (!access(capstone_lib_path, X_OK)) {
                perror(capstone_lib_path);
            }

            setenv("LD_PRELOAD", capstone_lib_path, 1);
            break;
            
        default:
    }

    if (execvp(arguments.child_argv[0], arguments.child_argv) == -1) {
        perror(arguments.child_argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

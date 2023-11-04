#include "main.h"

#include <argp.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>

const char *argp_program_version
    = "inproc rev 0.0000001";
const char *argp_program_bug_address
    = "<p.smolyakov@g.nsu.ru>";
static char doc[] 
    = "In-process firewall";
static char args_doc[]
    = "PATHNAME pathname_args...";

static struct argp_option options[] = { 
    { "method", 'm', "PRELOAD,AUDIT", OPTION_ARG_OPTIONAL, 
        "Specify method for intercepting calls. Default is preload." },
    { "output", 'o', "FILE", OPTION_ARG_OPTIONAL, 
        "Output logs to FILE instead of stdout." },
    { 0 }
};

struct arguments {
    enum { PRELOAD, AUDIT } mode;
    char *output_file_path;
    char **child_argv;
};

static error_t 
parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    fprintf(stderr, "id: %d arg: %s\n", state->, state->argv[state->next]);

    switch (key) {
        case 'm':
            assert(arg != NULL);
            write(2, arg, strlen(arg));
            if (!strcmp(arg, "preload")) {
                arguments->mode = PRELOAD;
            } else if (!strcmp(arg, "audit")) {
                arguments->mode = AUDIT;
            } else {
                argp_usage(state);
            }
            break;

        case 'o':
            arguments->output_file_path = arg;
            break;

        case ARGP_KEY_NO_ARGS:
            argp_usage(state);
            break;

        case ARGP_KEY_ARG:
            // write(2, state->argv[state->argc - 1], strlen(state->argv[state->argc - 1]));
            arguments->child_argv = &state->argv[state->argc - 1];
            return ARGP_KEY_END;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int 
main(int argc, char **argv) {
    struct arguments arguments;
    arguments.output_file_path = NULL;
    arguments.child_argv = NULL;
    arguments.mode = PRELOAD;

    argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &arguments);

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
    }

    if (arguments.output_file_path != NULL) {
        setenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR, arguments.output_file_path, 1);
    } else {
        unsetenv(INPROC_LOG_OUTPUT_FILE_ENV_VAR);
    }

    pid_t pid = fork();
    if (pid == 0 /*child*/) {
        if (execvp(arguments.child_argv[0], arguments.child_argv) == -1) {
            perror(arguments.child_argv[0]);
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0 /*parent*/) {
        int status;
        wait(&status);
        printf("\n~ '%s' exited with return code %u ~ \n\n", arguments.child_argv[0], WEXITSTATUS(status));
    } else {
        perror("unforkable");
    }
    
    return EXIT_SUCCESS;
}

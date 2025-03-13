#pragma once 

#include "envvars.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>


static int log_fd = -1;
static FILE *log_file = NULL;


static void
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
            fprintf(stderr, "!! log will be output to stdout\n\n"); fflush(stderr);

            log_fd = 1;
            log_file = stdout;
        } 
    }
}


#define OSSLTRACE_LOG(...) \
    assert(log_file != NULL); \
    fprintf(log_file, __VA_ARGS__); \
    fflush(log_file);


#define OSSLTRACE_LOG_BUF(buf, num) \
    assert(log_file != NULL); \
    write(log_fd, buf, num);

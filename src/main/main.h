#pragma once

#include "../common.h"

// #define INPROC_DEFAULT_LIB_PATH "/usr/lib/libinproc_inject.so"
#define INPROC_AUDIT_ENV_VAR "INPROC_AUDIT_LIB_PATH"
#define INPROC_DEFAULT_AUDIT_LIB_PATH "build/libinproc_audit.so"

#define INPROC_PRELOAD_ENV_VAR "INPROC_PRELOAD_LIB_PATH"
#define INPROC_DEFAULT_PRELOAD_LIB_PATH "build/libinproc_preload.so"

#define INPROC_NAIVEPATCH_ENV_VAR "INPROC_NAIVEPATCH_LIB_PATH"
#define INPROC_DEFAULT_NAIVEPATCH_LIB_PATH "build/libinproc_naivepatch.so"

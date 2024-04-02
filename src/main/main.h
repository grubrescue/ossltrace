#pragma once

#include "../common.h"

#define OSSLTRACE_AUDIT_ENV_VAR "OSSLTRACE_AUDIT_LIB_PATH"
#ifndef OSSLTRACE_DEFAULT_AUDIT_LIB_PATH
    #pragma message "OSSLTRACE_DEFAULT_AUDIT_LIB_PATH not defined. Defaults to /usr/lib/libossltrace_audit.so"
    #define OSSLTRACE_DEFAULT_AUDIT_LIB_PATH "/usr/lib/libossltrace_audit.so"
#endif

#define OSSLTRACE_PRELOAD_ENV_VAR "OSSLTRACE_PRELOAD_LIB_PATH"
#ifndef OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH
    #pragma message "OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH not defined. Defaults to /usr/lib/libossltrace_preload.so"
    #define OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH "/usr/lib/libossltrace_preload.so"
#endif

#define OSSLTRACE_NAIVEPATCH_ENV_VAR "OSSLTRACE_NAIVEPATCH_LIB_PATH"
#ifndef OSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH
    #pragma message "OSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH not defined. Defaults to /usr/lib/libossltrace_naivepatch.so"
    #define OSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH "/usr/lib/libossltrace_naivepatch.so"
#endif

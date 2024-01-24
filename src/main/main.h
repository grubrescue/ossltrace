#pragma once

#include "../common.h"

#define OSSLTRACE_AUDIT_ENV_VAR "OSSLTRACE_AUDIT_LIB_PATH"
#define OSSLTRACE_DEFAULT_AUDIT_LIB_PATH "/usr/lib/libossltrace_audit.so"

#define OSSLTRACE_PRELOAD_ENV_VAR "OSSLTRACE_PRELOAD_LIB_PATH"
#define OSSLTRACE_DEFAULT_PRELOAD_LIB_PATH "/usr/lib/libossltrace_preload.so"

#define OSSLTRACE_NAIVEPATCH_ENV_VAR "OSSLTRACE_NAIVEPATCH_LIB_PATH"
#define OSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH "/usr/lib/libossltrace_naivepatch.so"
// todo: ifdefы, чтобы можно было при билде настраивать
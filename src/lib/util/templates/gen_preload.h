#pragma once

#include "concat.h"
#include "gen_hook.h"

#include <dlfcn.h>
#include <unistd.h>


#define PRELOAD_HOOK_BODY(ORIG_FUN, ...) \
    if (GET_ORIGINAL(ORIG_FUN) == NULL) { \
        SET_ORIGINAL(ORIG_FUN, dlsym(RTLD_NEXT, #ORIG_FUN)); \
    } \
    return INVOKE_HOOK(ORIG_FUN, __VA_ARGS__);


#define PRELOAD_CONSTRUCTOR(CONSTRUCTOR_FUNC, ...) \
    __attribute__((constructor)) \
    void \
    CAT2(CONSTRUCTOR_FUNC, constructor)() { \
        CONSTRUCTOR_FUNC(__VA_ARGS__); \
    }

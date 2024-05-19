#pragma once

#include "concat.h"
#include "gen_singleton.h"

#include <dlfcn.h>
#include <unistd.h>

#define TEMPLATE_PRELOAD_FUNC_BODY(HOOKED_FUN, ORIG_FUN, ...) \
    if (GET_ORIG_FUNC_PTR(ORIG_FUN) == NULL) { \
        SET_ORIG_FUNC_PTR(ORIG_FUN, dlsym(RTLD_NEXT, #ORIG_FUN)); \
    } \
    HOOKED_FUN(__VA_ARGS__);


#define TEMPLATE_PRELOAD_CONSTRUCTOR(CONSTRUCTOR_FUNC, ...) \
    __attribute__((constructor)) \
    void \
    CAT2(CONSTRUCTOR_FUNC, constructor)() { \
        CONSTRUCTOR_FUNC(__VA_ARGS__); \
    }

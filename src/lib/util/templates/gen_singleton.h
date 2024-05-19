#pragma once

#include "concat.h"
#include <sys/types.h>

#define GEN_FUNC_PTR_SINGLETON(FUNC_NAME) \
    volatile void ** \
    CAT2(FUNC_NAME, singleton)() { \
        volatile static void *FUNC_NAME = NULL; \
        return &FUNC_NAME; \
    }

#define GET_FUNC_PTR(FUNC_NAME) \
    (*(CAT2(FUNC_NAME, singleton)()))

#define SET_FUNC_PTR(FUNC_NAME, PTR) \
    volatile void **CAT2(FUNC_NAME, tmpvar) = CAT2(FUNC_NAME, singleton)();\
    *CAT2(FUNC_NAME, tmpvar) = PTR

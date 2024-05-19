#pragma once

#include "concat.h"

#define FUNC_POSTFIX 102930128401

#define ORIG_FUNC_PTR_STORAGE(FUNC_NAME) \
    volatile void ** \
    CAT4(orig, FUNC_NAME, singleton, FUNC_POSTFIX)() { \
        volatile static void *ptr = NULL; \
        return &ptr; \
    }


#define _SINGLETON_CALL(FUNC_NAME) \
    CAT4(orig, FUNC_NAME, singleton, FUNC_POSTFIX)()


#define GET_ORIG_FUNC_PTR(FUNC_NAME) \
    *(_SINGLETON_CALL(FUNC_NAME))


#define SET_ORIG_FUNC_PTR(FUNC_NAME, PTR) \
    volatile void **CAT3(orig, FUNC_NAME, FUNC_POSTFIX) = _SINGLETON_CALL(FUNC_NAME);\
    *CAT3(orig, FUNC_NAME, FUNC_POSTFIX) = PTR

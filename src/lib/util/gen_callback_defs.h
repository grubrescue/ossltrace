#include "macro_funcs.h"

#ifndef GEN_SYM_NAME
#error "GEN_SYM_NAME must be defined before including gen_callback_defs.h" 
#endif

void * CAT2(original, GEN_SYM_NAME) = NULL;

void 
CAT2(set, GEN_SYM_NAME)(void *symbol) {
    CAT2(original, GEN_SYM_NAME) = symbol;
}

void *
CAT2(get, GEN_SYM_NAME)(void) {
    return CAT2(original, GEN_SYM_NAME);
}

#undef GEN_SYM_NAME

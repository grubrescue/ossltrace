#include "macro_funcs.h"

#ifndef GEN_SYM_NAME
#error "GEN_SYM_NAME must be defined before including gen_callback_decls.h" 
#endif

void 
CAT2(set, GEN_SYM_NAME)(void *symbol);

void *
CAT2(get, GEN_SYM_NAME)(void);

#undef GEN_SYM_NAME

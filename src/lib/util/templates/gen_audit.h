#pragma once

#include "concat.h"
#include "gen_singleton.h"

#include <link.h>
#include <stdio.h>

#define TEMPLATE_AUDIT_HEADER() \
    unsigned int \
    la_version(unsigned int version) { \
        return LAV_CURRENT; \
    }


#define TEMPLATE_AUDIT_CONSTRUCTOR(CONSTRUCTOR_FUNC, ...) \
    void la_preinit(uintptr_t *cookie) { \
        CONSTRUCTOR_FUNC(__VA_ARGS__); \
    }


#define TEMPLATE_AUDIT_LIB(LIB_NAME) \
    unsigned int \
    la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) { \
        if (strstr(map->l_name, #LIB_NAME) != NULL) { \
            return LA_FLG_BINDTO; \
        } else { \
            return LA_FLG_BINDFROM; \
        } \
    }

#define TEMPLATE_AUDIT_SYMBOLS \
    uintptr_t \
    la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook, \
            uintptr_t *defcook, unsigned int *flags, const char *symname)

#define TEMPLATE_AUDIT_SYMBOLS_ADD(HOOKED_SYM, ORIG_SYM) \
    if (!strcmp(symname, #ORIG_SYM)) { \
        if (GET_ORIG_FUNC_PTR(ORIG_SYM) == NULL) { \
            SET_ORIG_FUNC_PTR(ORIG_SYM, (void *) sym->st_value); \
        } \
        return (uintptr_t) HOOKED_SYM; \
    }

#define TEMPLATE_AUDIT_SYMBOLS_DEFAULT() \
    return sym->st_value;

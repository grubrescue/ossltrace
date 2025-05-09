#pragma once

#include <link.h>
#include <stdio.h>

#include "concat.h"
#include "gen_payload.h"

#define AUDIT_HEADER() \
  unsigned int la_version(unsigned int version) { return LAV_CURRENT; }

#define AUDIT_CONSTRUCTOR(CONSTRUCTOR_FUNC, ...) \
  void la_preinit(uintptr_t *cookie) { CONSTRUCTOR_FUNC(__VA_ARGS__); }

#define AUDIT_LIB(LIB_NAME)                                  \
  unsigned int la_objopen(struct link_map *map, Lmid_t lmid, \
                          uintptr_t *cookie) {               \
    if (strstr(map->l_name, #LIB_NAME) != NULL) {            \
      return LA_FLG_BINDTO;                                  \
    } else {                                                 \
      return LA_FLG_BINDFROM;                                \
    }                                                        \
  }

#define AUDIT_PAYLOADS                                                         \
  uintptr_t la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook, \
                         uintptr_t *defcook, unsigned int *flags,              \
                         const char *symname)

#define AUDIT_PAYLOAD_SYMBOL(ORIG_SYM)               \
  if (!strcmp(symname, #ORIG_SYM)) {                 \
    if (GET_ORIGINAL(ORIG_SYM) == NULL) {            \
      SET_ORIGINAL(ORIG_SYM, (void *)sym->st_value); \
    }                                                \
    return (uintptr_t)GET_PAYLOAD(ORIG_SYM);         \
  }

#define AUDIT_PAYLOAD_DEFAULT() return sym->st_value;

#define _GNU_SOURCE

#include "audit.h"

#include <link.h>
#include <stdio.h>

unsigned int 
la_version(unsigned int version) {
    // printf("la_version(): version = %u; LAV_CURRENT = %u\n",
    //         version, LAV_CURRENT);

    return LAV_CURRENT;
}

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
    // printf("la_objopen(): loading \"%s\"; lmid = %s; cookie=%p\n",
    //         map->l_name,
    //         (lmid == LM_ID_BASE) ?  "LM_ID_BASE" :
    //         (lmid == LM_ID_NEWLM) ? "LM_ID_NEWLM" :
    //         "???",
    //         cookie);

    if (strstr(map->l_name, "libssl") != NULL || strstr(map->l_name, "libcrypto") != NULL) {
        return LA_FLG_BINDTO | LA_FLG_BINDFROM;
    } else {
        return 0;
    }
}

uintptr_t
la_symbind64(Elf64_Sym * sym, unsigned int ndx, uintptr_t * refcook,
        uintptr_t * defcook, unsigned int * flags, const char * symname) {
    // printf("la_symbind64(): symname = %s; sym->st_value = %p\n",
    //         symname, (void *) sym->st_value);
    // printf("        ndx = %u; flags = %#x", ndx, *flags);
    // printf("; refcook = %p; defcook = %p\n", refcook, defcook);

    if (!strcmp(symname,"SSL_write")) {
        if (get_SSL_write_callback() == NULL) {
            set_SSL_write_callback((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_write;
    } else if (!strcmp(symname,"SSL_read")) {
        if (get_SSL_read_callback() == NULL) {
            set_SSL_read_callback((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_read;
    } else {
        return sym->st_value;
    }
}

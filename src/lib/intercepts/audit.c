#define _GNU_SOURCE

#include "audit.h"

#include <link.h>
#include <stdio.h>

unsigned int 
la_version(unsigned int version) {
    return LAV_CURRENT;
}

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
    if (strstr(map->l_name, "libssl") != NULL || strstr(map->l_name, "libcrypto") != NULL) {
        return LA_FLG_BINDTO | LA_FLG_BINDFROM;
    } else {
        return 0;
    }
}

uintptr_t
la_symbind64(Elf64_Sym *sym, unsigned int ndx, uintptr_t *refcook,
        uintptr_t *defcook, unsigned int *flags, const char *symname) {
    if (!strcmp(symname,"SSL_write")) {
        if (get_SSL_write() == NULL) {
            set_SSL_write((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_write;
    } else if (!strcmp(symname,"SSL_read")) {
        if (get_SSL_read() == NULL) {
            set_SSL_read((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_read;
    } else {
        return sym->st_value;
    }
}

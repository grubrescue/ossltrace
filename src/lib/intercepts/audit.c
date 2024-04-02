#define _GNU_SOURCE

#include "../hooks.h"

#include <link.h>
#include <stdio.h>



unsigned int 
la_version(unsigned int version) {
    return LAV_CURRENT;
}

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
    if (strstr(map->l_name, "libssl") != NULL) {
        return LA_FLG_BINDTO;
    } else {
        return LA_FLG_BINDFROM;
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
    } else if (!strcmp(symname,"SSL_get_verify_result")) {
        if (get_SSL_get_verify_result() == NULL) {
            set_SSL_get_verify_result((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_get_verify_result;
    } else if (!strcmp(symname,"SSL_CTX_set_verify")) {
        if (get_SSL_CTX_set_verify() == NULL) {
            set_SSL_CTX_set_verify((void *) sym->st_value);
        }

        return (uintptr_t) hooked_SSL_CTX_set_verify;
    } else {
        return sym->st_value;
    }
}

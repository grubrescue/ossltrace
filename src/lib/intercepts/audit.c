#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"

#include <link.h>
#include <stdio.h>


unsigned int 
la_version(unsigned int version) {
    return LAV_CURRENT;
}


void la_preinit(uintptr_t *cookie) {
    ossl_initialize();
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
        if (original_SSL_write == NULL) {
            original_SSL_write = (void *) sym->st_value;
        }

        return (uintptr_t) hooked_SSL_write;
    } else if (!strcmp(symname,"SSL_read")) {
        if (original_SSL_read == NULL) {
            original_SSL_read = (void *) sym->st_value;
        }

        return (uintptr_t) hooked_SSL_read;
    } else if (!strcmp(symname,"SSL_get_verify_result")) {
        if (original_SSL_get_verify_result == NULL) {
            original_SSL_get_verify_result = (void *) sym->st_value;
        }

        return (uintptr_t) hooked_SSL_get_verify_result;
    } else if (!strcmp(symname,"SSL_CTX_set_verify")) {
        if (original_SSL_CTX_set_verify == NULL) {
            original_SSL_CTX_set_verify = (void *) sym->st_value;
        }

        return (uintptr_t) hooked_SSL_CTX_set_verify;
    } else {
        return sym->st_value;
    }
}

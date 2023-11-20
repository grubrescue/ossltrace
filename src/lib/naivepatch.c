#define _GNU_SOURCE

#include "naivepatch.h"
#include "hooks.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <unistd.h>
#include <link.h>


static void *
monkey_patch(void * orig_ptr, void * payload_ptr) {
    static size_t PAGE_SIZE = -100500;

    if (PAGE_SIZE == -100500) {
        PAGE_SIZE = sysconf(_SC_PAGESIZE);
        if (PAGE_SIZE == -1) {
            perror("sysconf");
            exit(66);
        }
    }
    void *orig_page = (void *) (((size_t) orig_ptr) & ~(PAGE_SIZE - 1));

    int err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    typedef struct __attribute__((packed)) jumper {
        uint8_t jmp_qword_ptr_rip[6];
        size_t addr;
    } jumper; 
    
    jumper jmp_to_payload = {
        {0xff, 0x25, 0, 0, 0, 0}, (size_t) payload_ptr
    };

    jumper jmp_to_original = {
        {0xff, 0x25, 0, 0, 0, 0}, ((size_t) orig_ptr) + 0x11
    };

    void * jumper_to_orig_ptr = malloc(0x11 + sizeof(jmp_to_original));
    memcpy(jumper_to_orig_ptr, orig_ptr, 0x11); 
    memcpy(jumper_to_orig_ptr + 0x11, &jmp_to_original, sizeof(jmp_to_original));

    memcpy(orig_ptr + 0x04, &jmp_to_payload, sizeof(jmp_to_payload));

    err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    void * jumper_to_orig_page = (void *) (((size_t) jumper_to_orig_ptr) & ~(PAGE_SIZE - 1));
    err = mprotect(jumper_to_orig_page, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    return jumper_to_orig_ptr;
}

unsigned int 
la_version(unsigned int version) {
    return LAV_CURRENT;
}

// void
// la_preinit(uintptr_t * cookie) {
//     void * SSL_write_sym = dlsym(RTLD_DEFAULT, "SSL_write");
//     printf("addr of SSL_write: %p\n", SSL_write_sym); fflush(stdout);
//     if (SSL_write_sym != NULL) {
//         void * original_SSL_write = monkey_patch(SSL_write_sym, hooked_SSL_write);
//         set_SSL_write_callback(original_SSL_write);
//     }
//     // void * sym_next = dlsym(RTLD_NEXT, "SSL_write");
//     // printf("addr of next SSL_write: %p\n", sym); fflush(stdout);
// }

unsigned int
la_objopen(struct link_map *map, Lmid_t lmid, uintptr_t *cookie) {
    if (strstr(map->l_name, "libssl") != NULL) {
        printf("addr of map: %s\n", map->l_name); fflush(stdout);
        // void * SSL_write_sym = dlsym((void *) map, "SSL_write");


        // write(2, "yes\n\n\n", 7);
    } 

    return 0;
}

// typedef void *
// (*dlopen_callback)(const char *, int);

// dlopen_callback real_dlopen = NULL;

// void *dlopen(const char *filename, int flags) {
//     if (real_dlopen == NULL) {
//         real_dlopen = dlsym(RTLD_NEXT, "dlopen");
//     }

//     if (strstr(filename, "libssl") != NULL) {
//         fprintf(stderr, "sad.");
//     }
// }

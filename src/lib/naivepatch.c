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

    // for (int i = 0; i < 0x30; i++) {
    //     printf("%2.2x ", *((uint8_t*) (orig_ptr+i))); fflush(stdout);
    // }

    typedef struct __attribute__((packed)) jumper {
        uint8_t jmp_qword_ptr_rip[6];
        size_t addr;
    } jumper; 
    
    jumper jmp_to_payload = {
        {0xff, 0x25, 0, 0, 0, 0}, (size_t) payload_ptr
    };

    jumper jmp_to_original = {
        {0xff, 0x25, 0, 0, 0, 0}, ((size_t) orig_ptr) + 0x16
    };

    void * restored_orig_ptr = malloc(0x16 + sizeof(jmp_to_original));
    memcpy(restored_orig_ptr, orig_ptr, 0x16); 
    memcpy(restored_orig_ptr + 0x16, &jmp_to_original, sizeof(jmp_to_original));

    memcpy(orig_ptr + 0x04, &jmp_to_payload, sizeof(jmp_to_payload));

    // printf("\n");
    // for (int i = 0; i < 0x16 + sizeof(jmp_to_original); i++) {
    //     printf("%2.2x ", *((uint8_t*) (restored_orig_ptr+i))); fflush(stdout);
    // }

    // printf("\n");
    // for (int i = 0x16; i < 0x30; i++) {
    //     printf("%2.2x ", *((uint8_t*) (orig_ptr+i))); fflush(stdout);
    // }

    err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    void * restored_orig_page = (void *) (((size_t) restored_orig_ptr) & ~(PAGE_SIZE - 1));
    err = mprotect(restored_orig_page, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    return restored_orig_ptr;
}

typedef void *
(*dlopen_callback)(const char *, int);

dlopen_callback real_dlopen = NULL;

void *dlopen(const char *filename, int flags) {
    if (real_dlopen == NULL) {
        real_dlopen = dlsym(RTLD_NEXT, "dlopen");
    }

    if (strstr(filename, "libssl") != NULL) {
        fprintf(stderr, "sad.");
    }
}

typedef int 
(* SSL_write_callback) (SSL *, const void *, int);
static SSL_write_callback SSL_write_sym = NULL;

int 
SSL_write(SSL * ssl, const void * buf, int num) {
    if (SSL_write_sym == NULL) {
        SSL_write_sym = dlsym(RTLD_NEXT, "SSL_write");
        if (SSL_write_sym != NULL) {
            void * original_SSL_write = monkey_patch(SSL_write_sym, hooked_SSL_write);
            set_SSL_write_callback(original_SSL_write);
        }
    }

    return SSL_write_sym(ssl, buf, num);
}

// int 
// SSL_read(SSL * ssl, void * buf, int num) {
//     if (get_SSL_read_callback() == NULL) {
//         set_SSL_read_callback(dlsym(RTLD_NEXT, "SSL_read"));
//     }

//     hooked_SSL_read(ssl, buf, num);
// }

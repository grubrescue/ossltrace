#define _GNU_SOURCE

#include "../hooks.h"
#include "../initializer.h"
#include "../logger.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <unistd.h>
#include <link.h>

#include <capstone/capstone.h>

__attribute__((constructor)) 
void
initialize_constructor() {
    ossl_initialize();
}

static void 
disassembler_print(void *fun) {
    csh handle;
    cs_insn *insn;
    size_t count;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        return -1;
    }

    count = cs_disasm(handle, fun, 100, 0x1000, 0, &insn);
    OSSLTRACE_LOG("count is %d\n", count)
    
    if (count <= 0) {
        OSSLTRACE_LOG("couldn't disassemble")
    }

    size_t j;
    for (j = 0; j < count; j++) {
        if (insn)
        printf("0x%" PRIx64 ":\t%s\t\t%s   ::::   %d bytes\n", insn[j].address, insn[j].mnemonic, insn[j].op_str, insn[j].size);
    }

    cs_free(insn, count);
}


static void *
monkey_patch(void *orig_ptr, void *payload_ptr) {
    static size_t PAGE_SIZE = -100500;

    if (PAGE_SIZE == -100500) {
        PAGE_SIZE = sysconf(_SC_PAGESIZE);
        if (PAGE_SIZE == -1) {
            perror("sysconf");
            exit(66);
        }
    }
    void *orig_page = (void *) (((size_t) orig_ptr) & ~(PAGE_SIZE - 1));

    // int err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_WRITE);
    // if (err) {
    //     perror("mprotect");
    //     exit(66);
    // }

    disassembler_print(orig_ptr);

    // typedef struct __attribute__((packed)) jumper {
    //     uint8_t jmp_qword_ptr_rip[6];
    //     size_t addr;
    // } jumper; 

    // uint8_t endbr64[4] = {0xf3, 0x0f, 0x1e, 0xfa};
    
    // jumper jmp_to_payload = {
    //     {0xff, 0x25, 0, 0, 0, 0}, (size_t) payload_ptr
    // };

    // jumper jmp_to_original = {
    //     {0xff, 0x25, 0, 0, 0, 0}, ((size_t) orig_ptr) + 0x12
    // };

    // void *restored_orig_ptr = malloc(0x16 + sizeof(jmp_to_original));
    // void *restored_orig_ptr = mmap(NULL, 0x16 + sizeof(jmp_to_original), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // memcpy(restored_orig_ptr, orig_ptr, 0x16); 
    // memcpy(restored_orig_ptr + 0x16, &jmp_to_original, sizeof(jmp_to_original));

    // memcpy(orig_ptr + 0x04, &jmp_to_payload, sizeof(jmp_to_payload));
    // memcpy(orig_ptr + 0x12, endbr64, sizeof(endbr64));

    // err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    // if (err) {
    //     perror("mprotect");
    //     exit(66);
    // }

    // void *restored_orig_page = (void *) (((size_t) restored_orig_ptr) & ~(PAGE_SIZE - 1));
    // err = mprotect(restored_orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    // if (err) {
    //     perror("mprotect");
    //     exit(66);
    // }

    // return restored_orig_ptr;
    return NULL;
}



static void *real_dlopen = NULL;

void *dlopen(const char *filename, int flags) {
    typedef void *
    (*dlopen_callback)(const char *, int);

    write(2, "yes", 4);
    if (real_dlopen == NULL) {
        real_dlopen = dlsym(RTLD_NEXT, "dlopen");
    }

    if (filename != NULL && strstr(filename, "ssl") != NULL) {
        fprintf(stderr, "sad. found ssl lib"); fflush(stderr);
    } 
    
    return ((dlopen_callback) real_dlopen)(filename, flags);
}


static void *SSL_write_sym = NULL;

int 
SSL_write(SSL *ssl, const void *buf, int num) {
    typedef int 
    (*SSL_write_callback) (SSL *, const void *, int);

    if (SSL_write_sym == NULL) {
        SSL_write_sym = dlsym(RTLD_NEXT, "SSL_write");
        if (SSL_write_sym != NULL) {
            original_SSL_write = monkey_patch(SSL_write_sym, hooked_SSL_write);
        }
    }

    return ((SSL_write_callback) SSL_write_sym)(ssl, buf, num);
}


static void *SSL_read_sym = NULL;

int 
SSL_read(SSL *ssl, void *buf, int num) {
    typedef int 
    (*SSL_read_callback) (SSL *, void *, int);

    if (SSL_read_sym == NULL) {
        SSL_read_sym = dlsym(RTLD_NEXT, "SSL_read");
        if (SSL_read_sym != NULL) {
            original_SSL_read = monkey_patch(SSL_read_sym, hooked_SSL_read);
        }
    }

    return ((SSL_read_callback) SSL_read_sym)(ssl, buf, num);
}

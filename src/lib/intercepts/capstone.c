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

struct __attribute__((packed)) LJmp {
    uint8_t jmp_qword_ptr_rip[6];
    size_t addr;
}; 

struct PatchInfo {
    int err;
    size_t copyStart;
    size_t copyEnd;
};

#define OSSLTRACE_MAX_INSTRUCTIONS_TO_DISASM 40 
#define OSSLTRACE_LJMP_INSTR_SIZE sizeof(struct LJmp)

#define JMP_QWORD_RIP(addr) \
    (struct LJmp) {{0xff, 0x25, 0, 0, 0, 0}, ((size_t) addr)}

__attribute__((constructor)) 
void
initialize_constructor() {
    ossl_initialize();
}

static struct PatchInfo
disassembler_print(void *fun) {
    csh handle;
    cs_insn *insn;
    size_t count;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        OSSLTRACE_LOG("some problem while initializing capstone")
        return (struct PatchInfo) {.err = -1};
    }

    count = cs_disasm(handle, fun, 100, 0x00, OSSLTRACE_MAX_INSTRUCTIONS_TO_DISASM, &insn);
    if (count <= 0) {
        OSSLTRACE_LOG("couldn't disassemble")
        return (struct PatchInfo) {.err = -1};
    }

    size_t copyStart = 0;
    size_t copyEnd = 0;

    for (cs_insn *i = insn; i < insn + count; i++) {
        // printf("0x%" PRIx64 ":\t%s\t\t%s   ::::   %d bytes\n", i->address, i->mnemonic, i->op_str, i->size);
        if (i->id == X86_INS_ENDBR64) {
            copyStart += i->size;
            copyEnd += i->size;
            continue;
        }

        copyEnd += i->size;
        
        if (copyEnd - copyStart >= OSSLTRACE_LJMP_INSTR_SIZE) {
            break;
        }
    }

    cs_free(insn, count);

    return (struct PatchInfo) {.err = 0, .copyStart = copyStart, .copyEnd = copyEnd};
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

    int err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_WRITE);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    struct PatchInfo patchInfo = disassembler_print(orig_ptr);

    struct LJmp jmp_to_payload = JMP_QWORD_RIP(payload_ptr);
    struct LJmp jmp_to_original = JMP_QWORD_RIP(((size_t) orig_ptr) + patchInfo.copyEnd);

    void *restored_orig_ptr = mmap(NULL, patchInfo.copyEnd + OSSLTRACE_LJMP_INSTR_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(restored_orig_ptr, orig_ptr, patchInfo.copyEnd); 
    memcpy(restored_orig_ptr + patchInfo.copyEnd, &jmp_to_original, OSSLTRACE_LJMP_INSTR_SIZE);

    memcpy(orig_ptr + patchInfo.copyStart, &jmp_to_payload, OSSLTRACE_LJMP_INSTR_SIZE);

    err = mprotect(orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    void *restored_orig_page = (void *) (((size_t) restored_orig_ptr) & ~(PAGE_SIZE - 1));
    err = mprotect(restored_orig_page, PAGE_SIZE, PROT_READ | PROT_EXEC);
    if (err) {
        perror("mprotect");
        exit(66);
    }

    return restored_orig_ptr;
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

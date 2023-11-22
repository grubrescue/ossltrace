ifeq ($(INPROC_AUDIT_LIB_NAME),)
INPROC_AUDIT_LIB_NAME := libinproc_audit.so
endif

ifeq ($(INPROC_PRELOAD_LIB_NAME),)
INPROC_PRELOAD_LIB_NAME := libinproc_preload.so
endif

ifeq ($(INPROC_NAIVEPATCH_LIB_NAME),)
INPROC_NAIVEPATCH_LIB_NAME := libinproc_naivepatch.so
endif

ifeq ($(INPROC_EXECUTABLE_NAME),)
INPROC_EXECUTABLE_NAME := inproc
endif

ifeq ($(INPROC_BUILD_DIR),)
INPROC_BUILD_DIR := build
endif

CC=gcc

all: pre executable libs 

pre:
	mkdir -p $(INPROC_BUILD_DIR)
	mkdir -p $(INPROC_BUILD_DIR)/lib

executable: pre
	$(CC) -o $(INPROC_BUILD_DIR)/$(INPROC_EXECUTABLE_NAME) src/main/main.c

preload: pre
	$(CC) -shared -fPIC -o $(INPROC_BUILD_DIR)/lib/$(INPROC_PRELOAD_LIB_NAME) src/lib/preload.c src/lib/hooks.c -lssl

audit: pre
	$(CC) -shared -fPIC -o $(INPROC_BUILD_DIR)/lib/$(INPROC_AUDIT_LIB_NAME) src/lib/audit.c src/lib/hooks.c -lssl

naivepatch: pre
	$(CC) -shared -fPIC -o $(INPROC_BUILD_DIR)/lib/$(INPROC_NAIVEPATCH_LIB_NAME) src/lib/naivepatch.c src/lib/hooks.c -lssl

libs: preload audit naivepatch

clean: all
	rm -rfv $(INPROC_BUILD_DIR)/lib/

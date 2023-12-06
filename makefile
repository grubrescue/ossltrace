ifeq ($(OSSLTRACE_AUDIT_LIB_NAME),)
OSSLTRACE_AUDIT_LIB_NAME := libossltrace_audit.so
endif

ifeq ($(OSSLTRACE_PRELOAD_LIB_NAME),)
OSSLTRACE_PRELOAD_LIB_NAME := libossltrace_preload.so
endif

ifeq ($(OSSLTRACE_NAIVEPATCH_LIB_NAME),)
OSSLTRACE_NAIVEPATCH_LIB_NAME := libossltrace_naivepatch.so
endif

ifeq ($(OSSLTRACE_EXECUTABLE_NAME),)
OSSLTRACE_EXECUTABLE_NAME := ossltrace
endif

ifeq ($(OSSLTRACE_BUILD_DIR),)
OSSLTRACE_BUILD_DIR := build
endif

CC=gcc

all: pre executable libs 

pre:
	mkdir -p $(OSSLTRACE_BUILD_DIR)
	mkdir -p $(OSSLTRACE_BUILD_DIR)/lib

executable: pre
	$(CC) -o $(OSSLTRACE_BUILD_DIR)/$(OSSLTRACE_EXECUTABLE_NAME) src/main/main.c

preload: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_PRELOAD_LIB_NAME) src/lib/intercepts/preload.c src/lib/hooks.c -lssl

audit: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_AUDIT_LIB_NAME) src/lib/intercepts/audit.c src/lib/hooks.c -lssl

naivepatch: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_NAIVEPATCH_LIB_NAME) src/lib/intercepts/naivepatch.c src/lib/hooks.c -lssl

libs: preload audit naivepatch

clean: all
	rm -rfv $(OSSLTRACE_BUILD_DIR)/lib/

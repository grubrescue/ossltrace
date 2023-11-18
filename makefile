ifeq ($(INPROC_AUDIT_LIB_NAME),)
INPROC_AUDIT_LIB_NAME := libinproc_audit.so
endif

ifeq ($(INPROC_PRELOAD_LIB_NAME),)
INPROC_PRELOAD_LIB_NAME := libinproc_preload.so
endif

ifeq ($(INPROC_EXECUTABLE_NAME),)
INPROC_EXECUTABLE_NAME := inproc
endif

ifeq ($(INPROC_OUTPUT_DIR),)
INPROC_OUTPUT_DIR := build
endif

CC=gcc

all: pre executable preload audit
pre:
	mkdir -p $(INPROC_OUTPUT_DIR)
executable: pre
	$(CC) -o $(INPROC_OUTPUT_DIR)/$(INPROC_EXECUTABLE_NAME) src/main/main.c
preload: pre
	$(CC) -shared -fPIC -o $(INPROC_OUTPUT_DIR)/$(INPROC_PRELOAD_LIB_NAME) src/lib/preload.c src/lib/hooks.c -lssl
audit: pre
	$(CC) -shared -fPIC -o $(INPROC_OUTPUT_DIR)/$(INPROC_AUDIT_LIB_NAME) src/lib/audit.c src/lib/hooks.c -lssl

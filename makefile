# vars
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

ifeq ($(OSSLTRACE_LIBS_PATH),)
OSSLTRACE_LIBS_PATH := /usr/lib/
endif

CC=gcc


#tasks
#all: pre executable libs 

pre:
	mkdir -p $(OSSLTRACE_BUILD_DIR)
	mkdir -p $(OSSLTRACE_BUILD_DIR)/lib

build-executable: pre
	$(CC) -o $(OSSLTRACE_BUILD_DIR)/$(OSSLTRACE_EXECUTABLE_NAME) src/main/main.c

build-preload: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_PRELOAD_LIB_NAME) src/lib/intercepts/preload.c -lssl

build-audit: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_AUDIT_LIB_NAME) src/lib/intercepts/audit.c -lssl

build-naivepatch: pre
	$(CC) -shared -fPIC -o $(OSSLTRACE_BUILD_DIR)/lib/$(OSSLTRACE_NAIVEPATCH_LIB_NAME) src/lib/intercepts/naivepatch.c -lssl

build-libs: build-preload build-audit build-naivepatch

libs: build-libs # потом убрать
executable: build-executable

clean: build-libs
	rm -rfv $(OSSLTRACE_BUILD_DIR)/lib/

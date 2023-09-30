CC=gcc

ifeq ($(INPROC_LIB_NAME),)
INPROC_LIB_NAME := libinproc_inject.so
endif

ifeq ($(INPROC_EXECUTABLE_NAME),)
INPROC_EXECUTABLE_NAME := inproc
endif

ifeq ($(INPROC_OUTPUT_DIR),)
INPROC_OUTPUT_DIR := build
endif

all: pre app lib
pre:
	mkdir -p $(INPROC_OUTPUT_DIR)
app: pre
	$(CC) -o $(INPROC_OUTPUT_DIR)/$(INPROC_EXECUTABLE_NAME) src/main.c
lib: pre
	$(CC) -shared -fPIC -fvisibility=hidden -o $(INPROC_OUTPUT_DIR)/$(INPROC_LIB_NAME) src/lib.c 
	
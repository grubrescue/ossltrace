SHELL := /bin/bash

audit_lib_name = libossl_audit.so
preload_lib_name = libossl_preload.so
capstone_lib_name = libossl_capstone.so
loader_name = ossltrace
build_dir = build
libs_path = /usr/lib/

cc = gcc
runner_cflags = \
    -DOSSLTRACE_DEFAULT_AUDIT_LIB_PATH='"${libs_path}${audit_lib_name}"'  \
    -DOSSLTRACE_DEFAULT_PRELOAD_LIB_PATH='"${libs_path}${preload_lib_name}"' \
    -DOSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH='"${libs_path}${capstone_lib_name}"'

openssl_found := $(shell pkg-config --exists openssl && echo yes || echo no)
ifeq ($(openssl_found),yes)
    $(info OpenSSL library ok...)
else
    $(error OpenSSL is not installed; find a package like libssl-dev)
endif

capstone_found := $(shell pkg-config --exists capstone && echo yes || echo no)
ifeq ($(capstone_found),yes)
    $(info Capstone ok; building with Capstone disassembly framework...)
    runner_cflags += \
	-DOSSLTRACE_CAPSTONE_AVAILABLE=1
else
    $(warning Capstone is not installed; corresponding functionality won't be available)
endif


all: clean install

prepare:
	mkdir -p ${build_dir}
	mkdir -p ${build_dir}/lib

loader: prepare
	${cc} ${runner_cflags} -o ${build_dir}/${loader_name} src/main/main.c

libpreload.so: prepare
	${cc} -shared -fPIC -o ${build_dir}/lib/${preload_lib_name} src/lib/intercepts/preload.c -lssl -lpthread

libaudit.so: prepare
	${cc} -shared -fPIC -o ${build_dir}/lib/${audit_lib_name} src/lib/intercepts/audit.c -lssl -lpthread

libcapstone.so: prepare
	[[ "${capstone_found}" == "yes" ]] && \
	${cc} -shared -fPIC -o ${build_dir}/lib/${capstone_lib_name} src/lib/intercepts/capstone.c -lssl -lpthread -lcapstone \
	|| true

libs: libpreload.so libaudit.so libcapstone.so

build: libs loader

install-libs: libs
	install -v ${build_dir}/lib/* ${libs_path}

install: build install-libs clean

clean:
	rm -rfv ${build_dir}/lib/

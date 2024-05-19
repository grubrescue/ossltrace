audit_lib_name = libossl_audit.so
preload_lib_name = libossl_preload.so
capstone_lib_name = libossl_capstone.so
executable_name = ossltrace
build_dir = build
libs_path = /usr/lib/

cc = gcc
executable_cflags = \
	-DOSSLTRACE_DEFAULT_AUDIT_LIB_PATH='"$(libs_path)$(audit_lib_name)"'  \
	-DOSSLTRACE_DEFAULT_PRELOAD_LIB_PATH='"$(libs_path)$(preload_lib_name)"'

openssl_found := $(shell pkg-config --exists openssl && echo yes || echo no)
ifeq ($(openssl_found),no)
    $(error OpenSSL is not installed)
endif

capstone_found := $(shell pkg-config --exists capstone && echo yes || echo no)
ifeq ($(capstone_found),yes)
    executable_cflags += \
		-DOSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH='"$(libs_path)$(capstone_lib_name)"'
else
    $(warning Capstone is not installed; corresponding functionality won't be available)
endif


all: install # todo может не то?

prepare:
	mkdir -p $(build_dir)
	mkdir -p $(build_dir)/lib

executable: prepare
	$(cc) $(executable_cflags) -o $(build_dir)/$(executable_name) src/main/main.c

libpreload: prepare
	$(cc) -shared -fPIC -o $(build_dir)/lib/$(preload_lib_name) src/lib/intercepts/preload.c -lssl

libaudit: prepare
	$(cc) -shared -fPIC -o $(build_dir)/lib/$(audit_lib_name) src/lib/intercepts/audit.c -lssl

libcapstone: prepare
	pkg-config --exists capstone
	$(cc) -shared -fPIC -o $(build_dir)/lib/$(capstone_lib_name) src/lib/intercepts/capstone.c -lssl -lcapstone

libs: libpreload libaudit libcapstone

build: libs executable

install-libs: libs
	install -v $(build_dir)/lib/* $(libs_path)

install: build install-libs clean

clean:
	rm -rfv $(build_dir)/lib/

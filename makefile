OPENSSL_FOUND := $(shell pkg-config --exists openssl && echo yes || echo no)
ifeq ($(OPENSSL_FOUND),yes)
    CC_FLAGS += -DHAS_OPENSSL
else
    $(error OpenSSL is not installed)
endif

audit_lib_name = libaudit.so
preload_lib_name = libpreload.so
naivepatch_lib_name = libnaivepatch.so
executable_name = ossltrace
build_dir = build
libs_path = /usr/lib/

CC = gcc
EXECUTABLE_CFLAGS = -DOSSLTRACE_DEFAULT_AUDIT_LIB_PATH='"$(libs_path)$(audit_lib_name)"' -DOSSLTRACE_DEFAULT_PRELOAD_LIB_PATH='"$(libs_path)$(preload_lib_name)"' -DOSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH='"$(libs_path)$(naivepatch_lib_name)"'


all: install # todo может не совсем то что хотелось

pre:
	mkdir -p $(build_dir)
	mkdir -p $(build_dir)/lib

build-executable: pre
	$(CC) $(EXECUTABLE_CFLAGS) -o $(build_dir)/$(executable_name) src/main/main.c

build-preload: pre
	$(CC) -shared -fPIC -o $(build_dir)/lib/$(preload_lib_name) src/lib/intercepts/preload.c -lssl

build-audit: pre
	$(CC) -shared -fPIC -o $(build_dir)/lib/$(audit_lib_name) src/lib/intercepts/audit.c -lssl

build-naivepatch: pre
	$(CC) -shared -fPIC -o $(build_dir)/lib/$(naivepatch_lib_name) src/lib/intercepts/naivepatch.c -lssl

build-libs: build-preload build-audit build-naivepatch

build: build-libs build-executable

copy-libs: build-libs
	install -v $(build_dir)/lib/* $(libs_path)

install: build copy-libs clean

clean:
	rm -rfv $(build_dir)/lib/

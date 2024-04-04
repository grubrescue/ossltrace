OPENSSL_FOUND := $(shell pkg-config --exists openssl && echo yes || echo no)
ifeq ($(OPENSSL_FOUND),yes)
    CC_FLAGS += -DHAS_OPENSSL
else
    $(error OpenSSL is not installed)
endif

audit_lib_name = libossl_audit.so
preload_lib_name = libossl_preload.so
naivepatch_lib_name = libossl_naivepatch.so
capstone_lib_name = libossl_capstone.so
executable_name = ossltrace
build_dir = build
libs_path = /usr/lib/

CC = gcc
EXECUTABLE_CFLAGS = -DOSSLTRACE_DEFAULT_AUDIT_LIB_PATH='"$(libs_path)$(audit_lib_name)"' -DOSSLTRACE_DEFAULT_PRELOAD_LIB_PATH='"$(libs_path)$(preload_lib_name)"' -DOSSLTRACE_DEFAULT_NAIVEPATCH_LIB_PATH='"$(libs_path)$(naivepatch_lib_name)"' -DOSSLTRACE_DEFAULT_CAPSTONE_LIB_PATH='"$(libs_path)$(capstone_lib_name)"'


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

build-capstone: pre
	pkg-config --exists capstone
	$(CC) -shared -fPIC -o $(build_dir)/lib/$(capstone_lib_name) src/lib/intercepts/capstone.c -lssl -lcapstone

build-libs: build-preload build-audit build-naivepatch build-capstone

build: build-libs build-executable

copy-libs: build-libs
	install -v $(build_dir)/lib/* $(libs_path)

install: build copy-libs clean

clean:
	rm -rfv $(build_dir)/lib/

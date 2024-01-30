#!/bin/env sh

# Variables.
export OSSLTRACE_AUDIT_LIB_NAME=libossltrace_audit.so
export OSSLTRACE_PRELOAD_LIB_NAME=libossltrace_preload.so
export OSSLTRACE_NAIVEPATCH_LIB_NAME=libossltrace_naivepatch.so
export OSSLTRACE_EXECUTABLE_NAME=ossltrace
export OSSLTRACE_BUILD_DIR=build
export OSSLTRACE_LIBS_PATH=/usr/lib/

# Subroutines.

# $1 - errno
# $2 - exitcode
exit_if_unsuccessful() {
    if [ ! $1 -eq 0 ] ; then
        echo "Unsuccessful! Script will be terminated."
        exit $2
    fi
}

compile_and_mv_lib() {
    echo "~ Compiling libraries..."
    make -B libs 

    exit_if_unsuccessful $? 1

    echo "! Moving libs to $OSSLTRACE_LIBS_PATH; need sudo"
    sudo mv -v $OSSLTRACE_BUILD_DIR/lib/* $OSSLTRACE_LIBS_PATH

    exit_if_unsuccessful $? 2
}

compile_main() {
    echo "~ Compiling executable..."
    make -B executable

    exit_if_unsuccessful $? 3
}

clean() {
    echo "~ Cleaning up..."
    make clean
    exit_if_unsuccessful $? 4
}

# !!! The script itself.

echo "~ Running ossltrace install script..."

compile_and_mv_lib
compile_main
clean 

echo "~ Executable is located at $OSSLTRACE_BUILD_DIR/$OSSLTRACE_EXECUTABLE_NAME"
echo "~ Everything done!"

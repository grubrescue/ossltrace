#!/bin/env sh

# Variables.
export INPROC_AUDIT_LIB_NAME=libinproc_audit.so
export INPROC_PRELOAD_LIB_NAME=libinproc_preload.so
export INPROC_NAIVEPATCH_LIB_NAME=libinproc_naivepatch.so
export INPROC_EXECUTABLE_NAME=inproc
export INPROC_BUILD_DIR=build

# Subroutines.

# $1 - errno
# $2 - exitcode
exit_if_unsuccessful() {
    if [ ! $1 -eq 0 ] ; then
        echo "Unsuccessful! Exiting..."
        exit $2
    fi
}

compile_and_mv_lib() {
    echo "Compiling libraries"
    make -B libs 

    exit_if_unsuccessful $? 1

    echo "Moving libs to /usr/lib; need sudo"
    sudo mv -v $INPROC_BUILD_DIR/lib/* /usr/lib

    exit_if_unsuccessful $? 2
}

compile_main() {
    echo "Compiling main"
    make -B executable

    exit_if_unsuccessful $? 3
    echo "Done!"
}

clean() {
    make clean
    exit_if_unsuccessful $? 4
}

# !!! The script itself.

echo "Running inproc install script..."

compile_and_mv_lib
compile_main
clean 

echo "Executable is located at $INPROC_BUILD_DIR/$INPROC_EXECUTABLE_NAME"

echo "Truly done"

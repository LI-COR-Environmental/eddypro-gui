#!/usr/bin/env sh
# script to run the clang static analyzer from the build directory

# create dir for results
mkdir -p ../../test/clang_static_check

# require a cleanup to catch all the errors during compilation
make clean

# other options: -analyze-headers --keep-empty -stats
scan-build -V -v -k --keep-empty -stats -o ../../test/clang_static_check make -j4 -f Makefile.Debug

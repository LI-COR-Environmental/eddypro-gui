#!/usr/bin/env sh

# get all files added to the current branch from master, ignoring spaces,
# considering only c++ source files, showing the result on the console and
# logging to a local output file

DIR=$(pwd)
SRC_DIR=~/devel/eddypro/ep5/source/src
TEST_DIR=~/devel/eddypro/ep5/test/cpplint
PREFIX='../../../source/'

cd $SRC_DIR

git diff --diff-filter=A --name-only -w -b master... -- *.{h,cpp} | while read line; do echo "$PREFIX$line"; done | tee eddypro-added-files.txt

mv eddypro-added-files.txt $TEST_DIR


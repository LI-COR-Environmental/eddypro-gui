#!/usr/bin/env sh

# get all files added to the current branch from master, ignoring spaces,
# considering only c++ source files, showing the result on the console and
# logging to a local output file

VERSION=$1
SCRIPT_DIR=$(pwd)
REPO=~/devel/eddypro/gui/source/src
PREFIX="../../source/"
OUTPUT_FILE="new-files-in-$VERSION.txt"

cd $REPO
git diff --diff-filter=AM --name-only -w -b master... -- *.{h,cpp} | while read line; do echo ${PREFIX}; echo ${line}; done | tee $OUTPUT_FILE

# copy the output in the script dir
mv $OUTPUT_FILE $SCRIPT_DIR

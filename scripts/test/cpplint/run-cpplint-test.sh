#!/usr/bin/env sh

# get all files added to the current branch from master, ignoring spaces,
# considering only c++ source files, showing the result on the console and
# logging to a local output file

SCRIPT_VERSION=1.1
SCRIPT_DIR=$(pwd)
VERSION_TO_TEST=$1
SRC_DIR=~/devel/eddypro/gui/source/src
TEST_DIR=~/devel/eddypro/gui/test/cpplint
PREFIX='../../source/'
NEW_LIST_FILE="new-files-in-v$VERSION_TO_TEST.txt"
NEW_FILE_DIR="new-ep-v$VERSION_TO_TEST-files"
CPPLINT_EP="cpplint_eddypro.py"

print_sep() {
  echo "----------------------------------------"
}

usage() {
  echo
  echo "Get the list of EddyPro files added from the previous published version (master branch),"
  echo "and run cpplint on each one. Version $SCRIPT_VERSION."
  echo
  echo "Usage: $0 <eddypro-version>"
  echo
  echo "<eddypro_version> Specify current version number in short form, for example '6.0.0'"
  echo
  echo "Author: Antonio Forgione"
  echo "(c) 2014-2015 LI-COR Biosciences INC."
}

# begin output
print_sep
echo "### Running '$ $0 $@' in '$PWD' on $OSTYPE..."

# check args
if [ "$#" -lt 1 ]; then
  echo "Not enough arguments. Error 1."
  usage
  print_sep
  exit 2
fi

TEST_DIR_VERSION="$TEST_DIR/ep-$VERSION_TO_TEST"
RESULT_FILE="cpplint-eddypro-v$VERSION_TO_TEST-results.txt"

# create folder with version name in the test dir
cd $TEST_DIR
mkdir $TEST_DIR_VERSION 2> /dev/null

# get the list of EddyPro files added from the previous published version (master branch)
cd $SRC_DIR
git diff --diff-filter=A --name-only -w -b master... -- *.{h,cpp} | while read line; do echo "${PREFIX}${line}"; done | tee $NEW_LIST_FILE

# copy the file list in the test dir
# mv $NEW_LIST_FILE $SCRIPT_DIR

# run cpplint in the test dir
cd $SCRIPT_DIR
python $CPPLINT_EP $NEW_LIST_FILE 2> $RESULT_FILE

# move file list and results in the version folder
mv $NEW_LIST_FILE $TEST_DIR_VERSION
mv $RESULT_FILE $TEST_DIR_VERSION

cp $CPPLINT_EP $TEST_DIR_VERSION
cp $0 $TEST_DIR_VERSION

# move source files
shopt -s extglob
SUFFIXES='@(h|cpp)'
mv -f "$SCRIPT_DIR/"*.$SUFFIXES "$TEST_DIR_VERSION"

# move source files in a separate directory
cd $TEST_DIR_VERSION
mkdir $NEW_FILE_DIR 2> /dev/null
mv -f *.$SUFFIXES $NEW_FILE_DIR

# copy result files to docs server
# scp -rp ep-6.0.0 antonio@git.licor.com:/var/www/docs/Documents/38-12989-K/testresults/

#!/bin/sh

echo 'Update translation files...'

PWD=$(pwd)
echo "[pwd: $PWD]"

export PATH="~/Qt/5.7.0/5.7/clang_64/bin:$PATH"

lupdate -verbose -no-obsolete ../../source/eddypro_mac.pro
lrelease -verbose ../../source/eddypro_mac.pro

echo 'translations updated.'

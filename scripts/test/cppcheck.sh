#!/usr/bin/env sh

cppcheck --enable=all ../../src 2> cppcheck-results.txt

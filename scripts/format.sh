#!/bin/sh
CLANG_FORMAT=$1
find src/ -iname "*.h" -print0 -o -iname "*.cpp" -print0 | xargs -0 $CLANG_FORMAT -i -style=file

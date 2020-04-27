#!/bin/sh

files=$(find . | egrep "\.c$|\.h$" | tr '\n' ' ')
echo "files : $files"
clang-format -i -style="{BasedOnStyle: llvm, UseTab: Always, IndentWidth: 8}" $files

#!/bin/sh

set -xe

CC="clang"
CFLAGS="-Wall -Wextra -pedantic -ggdb"
SRC="main.c"

$CC $CFLAGS $CFLAGS $SRC -o main

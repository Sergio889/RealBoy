#!/bin/bash
mkdir compiledFiles_x86
cd compiledFiles_x86
../configure --disable-asm  --prefix=/dbg CPPFLAGS=-DDEBUG CXXFLAGS="-g -O0" CFLAGS="-g " CCASFLAGS="-g -O0"

#!/bin/bash
mkdir debug
cd debug
../configure --disable-asm  --prefix=/dbg CPPFLAGS=-DDEBUG CXXFLAGS="-g -O0" CFLAGS="-g" CCASFLAGS="-g O0"

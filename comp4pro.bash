#!/bin/bash
mkdir profiler
cd profiler
../configure --disable-asm  --prefix=/dbg CPPFLAGS= CXXFLAGS="-g -O0" CFLAGS=" -g -DPROFILER" CCASFLAGS=""

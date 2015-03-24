#!/bin/bash
mkdir profiler
cd profiler
../configure --disable-asm  --prefix=/dbg CPPFLAGS= CXXFLAGS="-g -O0" CFLAGS="-O3 -DPROFILER" CCASFLAGS=""

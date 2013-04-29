#!/usr/bin/env bash
gcc -Wall -Wno-unknown-pragmas -I. -DNDEBUG -O3 -fstrict-aliasing -ffast-math \
 -funroll-loops -fomit-frame-pointer -ffinite-math-only -std=c99 -fPIC \
 -c libimagequant.c mempool.c pam.c mediancut.c nearest.c viter.c blur.c
cc -fPIC -g -c -Wall liqua.c && cc -shared -Wl,-soname,liqua.so -o liqua.so liqua.o lib/*.o

#! /bin/bash
GPP47=/fs/buzz/cs714-fa11-022/localInstalls/bin/g++

mkdir src/o
mkdir build
mkdir build/lib
mkdir build/inc

# BUILD STATIC LIBRARY
# $GPP47 -I./inc -L./lib -c src/pbddop.cpp -o src/o/pbddop.o
# $GPP47 -I./inc -L./lib -c src/pbddcache.cpp -o src/o/pbddcache.o
# ar rcs build/lib/libpbdd.a src/o/pbddop.o src/o/pbddcache.o
# 
# rm -rf src/o/*

# BUILD SHARED LIBARY
$GPP47 -I./inc -L./lib -c -fPIC src/pbddop.cpp -o src/o/pbddop.o
$GPP47 -I./inc -L./lib -c -fPIC src/pbddcache.cpp -o src/o/pbddcache.o
$GPP47 -shared -Wl,-soname,build/lib/libpbdd.so -o build/lib/libpbdd.so src/o/pbddcache.o src/o/pbddop.o

rm -rf src/o

# COPY HEADER
cp src/pbdd.h build/inc/


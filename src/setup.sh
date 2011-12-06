#! /bin/bash
GPP47=/fs/buzz/cs714-fa11-022/localInstalls/bin/g++

$GPP47 -c -fPIC pbddop.cpp -o pbddop.o
$GPP47 -c -fPIC pbddcache.cpp -o pbddcache.o
$GPP47 -shared -Wl,-soname,libpbdd.so -o libpbdd.so.1  pbddcache.o pbddop.o

cp libpbdd.so.1 ~/localInstalls/lib/
cp pbdd.h ~/localInstalls/include/

#! /bin/bash
GPP47=/fs/buzz/cs714-fa11-022/localInstalls/bin/g++

PBDD_ROOT=~/pbdd
INC_DIR=$PBDD_ROOT/inc
LIB_DIR=$PBDD_ROOT/lib
BLD_DIR=$PBDD_ROOT/build
SRC_DIR=$PBDD_ROOT/src
TMP_DIR=$PBDD_ROOT/src/o

mkdir -p $TMP_DIR
mkdir -p $BLD_DIR
mkdir -p $BLD_DIR/lib
mkdir -p $BLD_DIR/inc

# BUILD SHARED LIBARY
$GPP47 -I$INC_DIR -L$LIB_DIR -c -fPIC $SRC_DIR/pbddop.cpp -o $TMP_DIR/pbddop.o
$GPP47 -I$INC_DIR -L$LIB_DIR -c -fPIC $SRC_DIR/pbddcache.cpp -o $TMP_DIR/pbddcache.o
$GPP47 -shared -Wl,-soname,$BLD_DIR/lib/libpbdd.so \
	-o $BLD_DIR/lib/libpbdd.so $TMP_DIR/pbddcache.o $TMP_DIR/pbddop.o

rm -rf $TMP_DIR

# COPY HEADER
cp $SRC_DIR/pbdd.h $BLD_DIR/inc/


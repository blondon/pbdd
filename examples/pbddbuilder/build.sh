#! /bin/bash
GPP47=/fs/buzz/cs714-fa11-022/localInstalls/bin/g++

PBDD_ROOT=~/pbdd
LIB_DIR=$PBDD_ROOT/lib
INC_DIR=$PBDD_ROOT/inc
BLD_DIR=$PBDD_ROOT/build

mkdir $BLD_DIR/bin

echo "Building formulatest ..."
$GPP47	-I$INC_DIR/ -I$INC_DIR/buddy -I$INC_DIR/tbb -I$BLD_DIR/inc \
		-L$LIB_DIR/boolstuff -lboolstuff-0.1 -L$LIB_DIR/buddy -lbdd -L$LIB_DIR/tbb -ltbb \
		-L$BLD_DIR/lib -lpbdd -lcilkrts -std=c++0x \
		parser.cpp variableorderer.cpp traverser.cpp ptraverser.cpp \
		formulatest.cpp -o $BLD_DIR/bin/formulatest

echo "Building dnftest ..."
$GPP47	-I$INC_DIR/ -I$INC_DIR/buddy -I$INC_DIR/tbb -I$BLD_DIR/inc \
		-L$LIB_DIR/boolstuff -lboolstuff-0.1 -L$LIB_DIR/buddy -lbdd -L$LIB_DIR/tbb -ltbb \
		-L$BLD_DIR/lib -lpbdd -lcilkrts -std=c++0x \
		parser.cpp variableorderer.cpp traverser.cpp ptraverser.cpp \
		dnftest.cpp -o $BLD_DIR/bin/dnftest

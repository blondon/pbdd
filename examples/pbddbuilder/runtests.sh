#!/bin/bash

time CILK_NWORKERS=1 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 1 100000
time CILK_NWORKERS=1 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 2 100000
time CILK_NWORKERS=2 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 2 100000
time CILK_NWORKERS=4 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 2 100000
time CILK_NWORKERS=8 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 2 100000
time CILK_NWORKERS=16 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 2 100000
time CILK_NWORKERS=1 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 3 100000
time CILK_NWORKERS=2 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 3 100000
time CILK_NWORKERS=4 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 3 100000
time CILK_NWORKERS=8 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 3 100000
time CILK_NWORKERS=16 ../../build/bin/dnftest ../../build/bin/testCases/factor_01/query2_01.txt 3 100000

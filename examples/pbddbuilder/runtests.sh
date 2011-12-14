#!/bin/bash

# increase size of the stack
# NOTE: might have to be set from command line manually
ulimit -s 32000

DNFTEST="../../build/bin/dnftest"

for i in 0001 0005 001 #005 01
do
	for j in 2 6 8 #16
	do
		echo ""
		echo "FACTOR $i QUERY $j"
		QPATH="../../build/bin/testCases/factor_${i}/query${j}_${i}.txt"
		$DNFTEST $QPATH 1 100000
# 		for k in 1 2 4 8 16
# 		do
# 			echo ""
# 			echo "NUM THREADS $k"
# 			CILK_NWORKERS=${k} $DNFTEST $QPATH 2 100000
# 		done
		for k in 1 2 4 8 16
		do
			echo ""
			echo "NUM THREADS $k"
			CILK_NWORKERS=${k} $DNFTEST $QPATH 3 100000
		done
	done
done

#!/bin/bash
for(( i = 1; i<=11; i++))
do
	for((j = 1; j<=128; j++))
	do
		echo $i $j
		diff -b -B -E <(./test in$i $j rfile) <(./mmu -a w -o Oxyfa -f $j in$i rfile)
	done
done
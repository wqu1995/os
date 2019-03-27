#!/bin/bash
for(( i = 0; i<=6; i++))
do
    diff -b -B -E <(./a input$i  rfile -sF) <(./sched -s F input$i rfile)
    diff -b -B -E <(./a -s L input$i  rfile) <(./sched -s L input$i rfile)
    diff -b -B -E <(./a input$i -sS rfile ) <(./sched -s S input$i rfile)


done

for((i = 1; i<100; i++))
do
	for((j = 0; j<=6; j++))
	do
		echo $i $j
		diff -b -B -E <(./a input$j -sR$i rfile ) <(./sched -s R$i input$j rfile)
	done
done

for(( i = 1; i<=100; i+=5))
do
	for((j = 1; j<100; j+=5))
	do
		for((k = 0; k<6; k++))
		do
			echo $i $j $k
			diff -b -B -E <(./a input$k  rfile -sP$i:$j) <(./sched -s P$i:$j input$k rfile)
			diff -b -B -E <(./a input$k  rfile -sE$i:$j) <(./sched -s E$i:$j input$k rfile)
		done
	done


done
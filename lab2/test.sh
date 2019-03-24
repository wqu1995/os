#!/bin/bash
for(( i = 0; i<=6; i++))
do
    diff -b -B -E <(./a input$i) <(./sched -v -t -e -s P5:3 input$i rfile)
done

#!/bin/bash
for(( i = 0; i<=6; i++))
do
    diff -b -B -E <(./a input$i  rfile -sE2:5) <(./sched -s E2:5 input$i rfile)
done

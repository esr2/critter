#!/bin/bash

PATHS=(
 "/Users/erinrosenbaum/Documents/Senior_Year/Thesis/ish/fall07/student_17"
 "/Users/erinrosenbaum/Documents/Senior_Year/Thesis/ish/fall07/student_94"
 "/Users/erinrosenbaum/Documents/Senior_Year/Thesis/ish/spr09/student_12"
 "/Users/erinrosenbaum/Documents/Senior_Year/Thesis/ish/spr03/student_23"
 )

PROGRAM="/Users/erinrosenbaum/Documents/Senior_Year/Thesis/repository/trunk/a.out"

echo $(date) >&2

for item in ${PATHS[*]}
do
	cd $item
	echo "============================================================" >&2
	echo $item >&2
	echo "------------------------------------------------------------" >&2
  $PROGRAM *.c
done


cd "/Users/erinrosenbaum/Documents/Senior_Year/Thesis/repository/trunk"


#!/bin/bash

single_files=""
full_files=""
nr_element=0

for fichier in single/*
do
	single_files=$single_files" "$fichier
done
for fichier in full/*
do
	full_files=$full_files" "$fichier
	compte=$((compte + 1))
done

single_files=( $single_files )
full_files=( $full_files )

for ((id=0; id<$compte; id++))
do
	fichier=${single_files[$id]}
        numero=${fichier##*.}
	grep "are running" ${full_files[$id]} | grep -oE '[0-9]+' > out_of_full_$numero
	grep "are running" ${single_files[id]} | grep -oE '[0-9]+' > out_of_single_$numero
	python3 genererGrapque.py out_of_full_$numero out_of_single_$numero $numero > dpus_utilization_in_time_$numero.txt
done




#!/bin/bash

chaine="program"
for ((compte=1; compte <=4; compte++))
do
	gcc program.c -o  $chaine$compte
done

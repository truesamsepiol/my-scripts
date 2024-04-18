#!/bin/bash

# Creation automatisé repertoire pour chaque des applications

fichier=$(cat applications)

for nom in $fichier
do
	mkdir -p ../$nom/speudo-code/PrIM	
	mkdir -p ../$nom/speudo-code/vPrIM	

	mkdir -p ../$nom/description/PrIM
	mkdir -p ../$nom/description/vPrIM

	mkdir -p ../$nom/metrics

	touch ../$nom/metrics/excution-time-PrIM
	touch ../$nom/metrics/excution-time-vPrIM

	touch ../$nom/metrics/CPU-DPU-time-PrIM
	touch ../$nom/metrics/CPU-DPU-time-vPrIM

	touch ../$nom/metrics/DPU-CPU-time-PrIM
	touch ../$nom/metrics/DPU-CPU-time-vPrIM

	touch ../$nom/metrics/DPU-time-PrIM
	touch ../$nom/metrics/DPU-time-vPrIM

	touch ../$nom/metrics/CPU-time-PrIM
	touch ../$nom/metrics/CPU-time-vPrIM
done

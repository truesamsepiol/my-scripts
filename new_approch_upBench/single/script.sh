
for fichier in ../trace/* 
do 
	suffixe=${fichier#../*/trace}
	numero=${suffixe%.txt}
	echo "--- Debut lancement de la commande --> ./single_scheduler -f $fichier -n single_time_metric_$numero.txt > out.$numero"
	./single_scheduler -f $fichier -n single_time_metric_$numero.txt > out.$numero
	echo "--- Fin   lancement de la commande --> ./single_scheduler -f $fichier -n single_time_metric_$numero.txt > out.$numero"
done

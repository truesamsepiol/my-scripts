
for fichier in ../trace/* 
do 
	suffixe=${fichier#../*/trace}
	numero=${suffixe%.txt}
	echo "--- Debut lancement de la commande --> ./full_scheduler -f $fichier -n full_time_metric_$numero.txt > out.$numero"
	./full_scheduler -f $fichier -n full_time_metric_$numero.txt > out.$numero
	echo "--- Fin   lancement de la commande --> ./full_scheduler -f $fichier -n full_time_metric_$numero.txt > out.$numero"
done
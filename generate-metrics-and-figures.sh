#Put all metrics in the same file and format
#launch the script python who generate a figure in the same file

path=../benchmarks
names=$(ls $path)

if test -e metrics 
then
	rm metrics	
fi

for name in $names
do
	echo $name >> metrics
	echo -n $(cat $path/$name/metrics/CPU-DPU-time-PrIM),  >> metrics
	echo -n $(cat $path/$name/metrics/DPU-CPU-time-PrIM),  >> metrics
	echo  $(cat $path/$name/metrics/excution-time-PrIM)    >> metrics
	echo -n $(cat $path/$name/metrics/CPU-DPU-time-vPrIM), >> metrics
	echo -n $(cat $path/$name/metrics/DPU-CPU-time-vPrIM), >> metrics
	echo  $(cat $path/$name/metrics/excution-time-vPrIM)   >> metrics
done


/usr/bin/python3 generate-figures.py metrics

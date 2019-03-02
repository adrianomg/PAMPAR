#!/bin/bash
gcc -o geraGrafo geraGrafoDJ.c
source ../../inputDefault.sh

if [ ! -d "inputDJ" ]; then
  mkdir inputDJ
fi

mv geraGrafo inputDJ
cd inputDJ

for i in "${inputDJ[@]}"
do
	if [ ! -f  $i'.txt' ]; then
		./geraGrafo $i $i'.txt'
	else
		echo "Ja existe o arquivo $i.txt"
	fi
done

rm geraGrafo
cd ..

#!/bin/bash
gcc -o geraMatriz geraMatrizGL.c
source ../../inputDefault.sh

if [ ! -d "inputGL" ]; then
  mkdir inputGL
fi

mv geraMatriz inputGL
cd inputGL

for i in "${inputGL[@]}"
do
	if [ ! -f  $i'.txt' ]; then
		./geraMatriz $i $i'.txt'
	else
		echo "Ja existe o arquivo $i.txt"
	fi
done

rm geraMatriz
cd ..

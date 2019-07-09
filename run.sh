#!/bin/bash

host=`hostname`

source inputDefault.sh

apps=(DFT DJ DP GS HA JA MM NI OE PI TR)
vseq=("$inputDFT"
      "$inputDJ Apps/DJ/inputDJ/$inputDJ.txt"
      "$inputDP"
      "$inputGS"
      "$inputHA $inputHA"
      "$inputJA"
      "$inputMM"
      "$inputNI"
      "$inputOE"
      "$inputPI"
      "$inputTR"
      )

for step in `seq 1 $EXECNUMBER`; do
	date +"%d/%m/%Y %H:%M:%S"
	printf "  Running on $host \n"
	printf "  Step: $step \n\n"
	for i in ${!apps[*]}; do
		app=${apps[$i]}
		input=${vseq[$i]}
		date +"%d/%m/%Y %H:%M:%S"
		printf "    Application: $app\n"
		printf "    IPP: Sequential\n"	
		time=`./Apps/$app/sequential $input|grep -i time | awk {'print $3'}`
		printf "    Time: $time\n\n"
	done

	for th in "${threads[@]}"; do
		for i in ${!apps[*]}; do
			app=${apps[$i]}
			input=${vseq[$i]}
			date +"%d/%m/%Y %H:%M:%S"
			printf "    Application: $app\n"
			printf "    IPP: Pthread\n"
			printf "    Threads: $th\n"
			time=`./Apps/$app/pthread $th $input|grep -i time | awk {'print $3'}`
			printf "    Time: $time\n\n"
		done

		for i in ${!apps[*]}; do
			app=${apps[$i]}
			input=${vseq[$i]}
			date +"%d/%m/%Y %H:%M:%S"
			printf "    Application: $app\n"
			printf "    IPP: OpenMP\n"
			printf "    Threads: $th\n"
			time=`./Apps/$app/openmp $th $input|grep -i time | awk {'print $3'}`
			printf "    Time: $time\n\n"
		done

		for i in ${!apps[*]}; do
			app=${apps[$i]}
			input=${vseq[$i]}
			date +"%d/%m/%Y %H:%M:%S"
			printf "    Application: $app\n"
			printf "    IPP: MPI-1\n"
			printf "    Threads: $th\n"		
			time=`mpirun -np $th ./Apps/$app/mpi1 $input|grep -i time | awk {'print $3'}`
			printf "    Time: $time\n\n"
		done

		for i in ${!apps[*]}; do
			app=${apps[$i]}
			input=${vseq[$i]}
			date +"%d/%m/%Y %H:%M:%S"
			printf "    Application: $app\n"
			printf "    IPP: MPI-2\n"
			printf "    Threads: $th\n"
			time=`mpirun -np 1 ./Apps/$app/pai $(($th-1)) $input Apps/$app/filho |grep -i time | awk {'print $3'}`
			printf "    Time: $time\n\n"
		done		
	done
done

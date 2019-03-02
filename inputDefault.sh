#!/bin/bash

#Setup file

#set of parallel tasks to run
declare -a threads=("2" "4" "8" "16" "20")

#Number of executions
EXECNUMBER=1

#small, medium, large, debug
SIZE=medium



BENCHMARK="$PWD"

declare -a ipps=("PThreads" "OpenMP" "MPI-1" "MPI-2" "Sequential" "Ready" "All")
pthreadsIpp=0
openmpIpp=0
mpi1Ipp=0
mpi2Ipp=0
sequentialIpp=0

declare -a applications=("GS" "JA" "OE" "TR" "DFT" "DJ" "DP" "HA" "MM" "PI" "NI" "Ready" "All")
#GL=0
GS=0
JA=0
OE=0
TR=0
DFT=0
DJ=0
DP=0
HA=0
MM=0
PI=0
NI=0
#SH=0


if [[ $SIZE = "small" ]]; then
	declare -a inputGL=("512")
	declare -a inputGS=("512")
	declare -a inputJA=("1024")
	declare -a inputOE=("150000")
	declare -a inputTR=("1024")
	declare -a inputDFT=("16184")
	declare -a inputDJ=("1024")
	declare -a inputDP=("10000000000")
	declare -a inputHA=("50000")
	declare -a inputMM=("1024")
	declare -a inputPI=("1000000000")
	declare -a inputNI=("750000000")
elif [[ $SIZE = "medium" ]]; then
	declare -a inputGL=("2048")
	declare -a inputGS=("1024")
	declare -a inputJA=("2048")
	declare -a inputOE=("225000")
	declare -a inputTR=("2048")
	declare -a inputDFT=("32368")
	declare -a inputDJ=("1024")
	declare -a inputDP=("15000000000")
	declare -a inputHA=("100000")
	declare -a inputMM=("2048")
	declare -a inputPI=("4000000000")
	declare -a inputNI=("1000000000")
elif [[ $SIZE = "large" ]]; then		
	declare -a inputGL=("4096")
	declare -a inputGS=("4096")
	declare -a inputJA=("4096")
	declare -a inputOE=("300000")
	declare -a inputTR=("4096")
	declare -a inputDFT=("64736")
	declare -a inputDJ=("4096")
	declare -a inputDP=("20000000000")
	declare -a inputHA=("150000")
	declare -a inputMM=("4096")
	declare -a inputPI=("6000000000")
	declare -a inputNI=("1500000000")
elif [[ $SIZE = "debug" ]]; then
	declare -a inputGL=("256")
	declare -a inputGS=("256")
	declare -a inputJA=("256")
	declare -a inputOE=("1500")
	declare -a inputTR=("256")
	declare -a inputDFT=("256")
	declare -a inputDJ=("256")
	declare -a inputDP=("1000")
	declare -a inputHA=("1000")
	declare -a inputMM=("256")
	declare -a inputPI=("4000")
	declare -a inputNI=("1000")
	#declare -a inputSHWidth=("1080")
	#declare -a inputSHHeight=("1920")
else
	echo "Invalid Option!"
fi



#!/bin/bash

host=`hostname`

declare -a ipps=("PThreads" "OpenMP" "MPI-1" "MPI-2" "Sequential" "Ready" "All")
pthreadsIpp=0
openmpIpp=0
mpi1Ipp=0
mpi2Ipp=0
sequentialIpp=0

clear
echo "Which PPI do you want to perform?"
select ipp in "${ipps[@]}"
do
        case $ipp in
                "PThreads")pthreadsIpp=1;;
                "OpenMP")openmpIpp=1;;
                "MPI-1")mpi1Ipp=1;;
                "MPI-2")mpi2Ipp=1;;
                "Sequential")sequentialIpp=1;;
                "All")
                        pthreadsIpp=1
                        openmpIpp=1
                        mpi1Ipp=1
                        mpi2Ipp=1
                        sequentialIpp=1
                        break;;
                "Ready")break;;
                *) echo "Invalid Option!";;
        esac
done

clear

declare -a apps=("DFT" "DJ" "DP" "GS" "HA" "JA" "MM" "NI" "OE" "PI" "TR" "Ready" "All")

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

echo "Which applications do you want to run?"
select app in "${apps[@]}"
do
        case $app in
                "GL")GL=1;;
                "GS")GS=1;;
                "JA")JA=1;;
                "OE")OE=1;;
                "TR")TR=1;;
                "DFT")DFT=1;;
                "DJ")DJ=1;;
                "DP")DP=1;;
                "HA")HA=1;;
                "MM")MM=1;;
                "PI")PI=1;;
                "NI")NI=1;;
                "SH")SH=1;;
                "All")
                        GL=1
                        GS=1
                        JA=1
                        OE=1
                        TR=1
                        DFT=1
                        DJ=1
                        DP=1
                        HA=1
                        MM=1
                        PI=1
                        NI=1
                        SH=1
                        break;;
                "Ready")break;;
                *) echo "Invalid Option!";;
        esac
done

clear

if [[ $pthreadsIpp -eq 1 || $openmpIpp -eq 1 || $mpi1Ipp -eq 1 || $mpi2Ipp -eq 1 ]]
then
	echo "Set the amount of parallel tasks to used (MINIMUM 2). You can set a list separating the values by space (e.g. 2 8 16 20)."
	read -a TASKS
	for task in "${TASKS[@]}"
	do
		if [[ $task -eq 1 ]]
        	then
        	        printf "\n\nValue 1 for parallel tasks is not allowed.\nIt is required at least 2 tasks.\n\nStart again and choose only the sequential option for running with a single thread.\n\n"
        	        exit 1
        	fi
	done
fi

clear
echo "Set the number of executions."
read -e EXECNUMBER

clear
echo "Choose the size of the input set (small, medium, large, or test)."
read -e SIZE
re1='[Ss]mall|SMALL|[Mm]edium|MEDIUM|[Ll]arge|LARGE|[Tt]est|TEST'
if ! [[ $SIZE =~ $re1 ]] ; then
   echo "Invalid Size!"
   exit 1
fi


printf "Selected PPIs: "
for ipp in ${ipps[@]}
do
        if [[ $ipp -eq 1 ]]
        then
                printf "$ipp "
        fi
done

echo ""
printf "Selected Applications: "
for app in ${apps[@]}
do
        if [[ $app -eq 1 ]]
        then
                printf "$app "
        fi
done

echo ""
echo "Problem size: $SIZE"
echo "Number of executions: $EXECNUMBER"

printf "Tasks: "
for task in "${TASKS[@]}"
do
        printf "$task "
done

printf "\n\n"

echo "Confirm? (yes/no)"
read CONFIRM
re2='[Yy]es|YES|[Yy]'
if ! [[ $CONFIRM =~ $re2 ]] ; then
   exit 1
fi

clear

if [[ $SIZE = "small" || $SIZE = "Small" || $SIZE = "SMALL" ]]; then
	#declare -a inputGL=("512")
	declare -a inputGS=("512")
	declare -a inputJA=("1024")
	declare -a inputOE=("150000")
	declare -a inputTR=("1024")
	declare -a inputDFT=("16184")
	declare -a inputDJ=("1024")
	declare -a inputDP=("10000000000")
	declare -a inputHA=("50000")
	declare -a inputMM=("2048")
	declare -a inputPI=("1000000000")
	declare -a inputNI=("750000000")
elif [[ $SIZE = "medium" || $SIZE = "Medium" || $SIZE = "MEDIUM" ]]; then
	#declare -a inputGL=("2048")
	declare -a inputGS=("1024")
	declare -a inputJA=("2048")
	declare -a inputOE=("225000")
	declare -a inputTR=("2048")
	declare -a inputDFT=("32368")
	declare -a inputDJ=("2048")
	declare -a inputDP=("15000000000")
	declare -a inputHA=("100000")
	declare -a inputMM=("4096")
	declare -a inputPI=("4000000000")
	declare -a inputNI=("1000000000")
elif [[ $SIZE = "large" || $SIZE = "Large" || $SIZE = "LARGE" ]]; then		
	#declare -a inputGL=("4096")
	declare -a inputGS=("4096")
	declare -a inputJA=("4096")
	declare -a inputOE=("300000")
	declare -a inputTR=("4096")
	declare -a inputDFT=("64736")
	declare -a inputDJ=("4096")
	declare -a inputDP=("20000000000")
	declare -a inputHA=("150000")
	declare -a inputMM=("8192")
	declare -a inputPI=("6000000000")
	declare -a inputNI=("1500000000")
elif [[ $SIZE = "test" || $SIZE = "Test" || $SIZE = "TEST" ]]; then
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
	
	if [[ $sequentialIpp -eq 1 ]]
	then
		for app in ${apps[@]}; do
			if [[ $app -eq 1 ]]
			then
				let "n=(`echo ${apps[@]} | tr -s " " "\n" | grep -n "$app" | cut -d":" -f 1`)-1" #get the app index
				input=${vseq[$n]}
				date +"%d/%m/%Y %H:%M:%S"
				printf "    Application: $app\n"
				printf "    IPP:\t Sequential\n"
				printf "    Input:\t $input\n"
				time=`./Apps/$app/sequential $input|grep -i time | awk {'print $3'}`
				printf "    Time:\t $time\n\n"
			fi
		done
	fi
	for th in "${TASKS[@]}"; do
		if [[ $pthreadsIpp -eq 1 ]]
		then
			for app in ${apps[@]}; do
				if [[ $app -eq 1 ]]
				then
					let "n=(`echo ${apps[@]} | tr -s " " "\n" | grep -n "$app" | cut -d":" -f 1`)-1" #get the app index
					input=${vseq[$n]}
					date +"%d/%m/%Y %H:%M:%S"
					printf "    Application: $app\n"
					printf "    IPP:\t Pthread\n"
					printf "    Threads:\t $th\n"
					printf "    Input:\t $input\n"
					time=`./Apps/$app/pthread $th $input|grep -i time | awk {'print $3'}`
					printf "    Time:\t $time\n\n"
				fi
			done
		fi
		
		if [[ $openmpIpp -eq 1 ]]
		then
			for app in ${apps[@]}; do
				if [[ $app -eq 1 ]]
				then
					let "n=(`echo ${apps[@]} | tr -s " " "\n" | grep -n "$app" | cut -d":" -f 1`)-1" #get the app index
					input=${vseq[$n]}
					date +"%d/%m/%Y %H:%M:%S"
					printf "    Application: $app\n"
					printf "    IPP:\t OpenMP\n"
					printf "    Threads:\t $th\n"
					printf "    Input:\t $input\n"
					time=`./Apps/$app/openmp $th $input|grep -i time | awk {'print $3'}`
					printf "    Time:\t $time\n\n"
				fi
			done
		fi
		
		if [[ $mpi1Ipp -eq 1 ]]
		then
			for app in ${apps[@]}; do
				if [[ $app -eq 1 ]]
				then
					let "n=(`echo ${apps[@]} | tr -s " " "\n" | grep -n "$app" | cut -d":" -f 1`)-1" #get the app index
					input=${vseq[$n]}
					date +"%d/%m/%Y %H:%M:%S"
					printf "    Application: $app\n"
					printf "    IPP:\t MPI-1\n"
					printf "    Threads:\t $th\n"
					printf "    Input:\t $input\n"		
					time=`mpirun -np $th ./Apps/$app/mpi1 $input|grep -i time | awk {'print $3'}`
					printf "    Time:\t $time\n\n"
				fi
			done
		fi
		
		if [[ $mpi2Ipp -eq 1 ]]
		then
			for app in ${apps[@]}; do
				if [[ $app -eq 1 ]]
				then
					let "n=(`echo ${apps[@]} | tr -s " " "\n" | grep -n "$app" | cut -d":" -f 1`)-1" #get the app index
					input=${vseq[$n]}
					date +"%d/%m/%Y %H:%M:%S"
					printf "    Application: $app\n"
					printf "    IPP:\t MPI-2\n"
					printf "    Threads:\t $th\n"
					printf "    Input:\t $input\n"
					time=`mpirun -np 1 ./Apps/$app/pai $(($th-1)) $input ./Apps/$app/filho |grep -i time | awk {'print $3'}`
					printf "    Time:\t $time\n\n"
				fi
			done
		fi	
	done
done

date +"%d/%m/%Y %H:%M:%S"
echo "DONE!"

exit

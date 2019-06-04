#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

long int N = 0, *vetIndices = NULL, *vetor = NULL, size = 0, indice = 0;
MPI_Comm interComm;
MPI_Request request[128];

void sortPar(int rank){
	long int aux, i;
	for(i=vetIndices[(rank*size)+rank]; i<vetIndices[size+rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void sortImpar(int rank){
	long int aux, i;
	for(i=vetIndices[(size*2)+rank]; i<vetIndices[(size*3)+rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void defineChunk(int size){
	int i;
	long int aux = N/size;
	
	for(i=0; i<size; i++){
		if(i==0)
			vetIndices[i] = 0;
		else
			vetIndices[i] = i*aux;
	}
	for(i=0; i<size; i++){
		vetIndices[i+size] = ((i+1)*aux)-1;
	}
	for(i=0; i<size; i++){
		if(i==0)
			vetIndices[i+(2*size)] = 1;
		else
			vetIndices[i+(2*size)] = (i*aux)-1;
	}
	for(i=0; i<size; i++){
		if (i==size-1)
			vetIndices[i+(3*size)] = N-1;
		else
			vetIndices[i+(3*size)] = ((i+1)*aux)-2;
	}
}

void bubbleSort(){
	int i, source=0;
	int aux = N/size;
	int id = 99;
			
	for(i=0;i<(N/2)+1;i++){
		sortPar(0);
		MPI_Send(&vetor[aux-1], 1, MPI_INT, 0, id, interComm);
		MPI_Irecv(&vetor[aux-1], 1, MPI_INT, 0, id, interComm, &request[0]);
		sortImpar(0);
		MPI_Wait(&request[0], MPI_STATUS_IGNORE);
	}
	if(size==2){
		MPI_Recv(&vetor[aux-1], (N-(aux-1))+1, MPI_INT, 0, id, interComm, MPI_STATUS_IGNORE);
	}else{
		for(i=0; i<size-1; i++){
			MPI_Irecv(&vetor[((i+1)*aux)-1], aux, MPI_INT, i, id+(i*100), interComm, &request[i]);
		}
		for(i=0;i<size-1;i++){
			MPI_Waitany(size-1, request, &source, MPI_STATUS_IGNORE);
		}
	}
}

void leEntrada(){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 200000 \"$PWD/child\"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int numFilhos = atoi(argv[1]);

	size = numFilhos+1;

	MPI_Init(&argc, &argv);
  	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	long long valor;
				
	N = atoi(argv[2]);
	char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);

	vetor = malloc(sizeof(long int)*N);
	vetIndices = malloc(sizeof(long int)*(size)*4);
	defineChunk(numFilhos+1);
				
	leEntrada();
	int i, err[1];
		
	for(i=argc;i<7;i++){
		argv[i] = malloc(sizeof(char)*50);
	}

	sprintf(argv[4], "%d", numFilhos);
	sprintf(argv[5], "%ld", indice);
	argv[6] = NULL;

	MPI_Comm_spawn(bin, argv, numFilhos, localInfo, 0, MPI_COMM_SELF, &interComm, err);
	
	for(i=0;i<numFilhos;i++){
		MPI_Send(&vetIndices[0], size*4, MPI_INT, i, 99, interComm);
	
	}

	bubbleSort();

	MPI_Finalize();
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
	
	return 0;
}

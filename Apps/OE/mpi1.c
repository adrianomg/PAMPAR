#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int N, *vetor, *vetIniImpar, *vetFimImpar, *vetIniPar, *vetFimPar, rank2, total, indice;
MPI_Request req[128];

void sortPar(int rank){
	int aux, i;
	for(i=vetIniPar[rank]; i<vetFimPar[rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void sortImpar(int rank){
	int aux, i;
	for(i=vetIniImpar[rank]; i<vetFimImpar[rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void defineChunk(int size){
	int i;
	int aux = N/size;
	
	for(i=0; i<size; i++){
		vetIniPar[i] = i*aux;
		vetFimPar[i] = (((i+1)*aux)-1);

		if(i == 0){
			vetIniImpar[i] = ((i*aux)+1);
			vetFimImpar[i] = (((i+1)*aux)-2);
		}
		else if(i == size-1){
			vetIniImpar[i] = ((i*aux)-1);
			vetFimImpar[i] = (((i+1)*aux)-1);
		}
		else if(i!=0 && i!=size-1){
			vetIniImpar[i] = ((i*aux)-1);
			vetFimImpar[i] = (((i+1)*aux)-2);
		}
	}
}

void bubbleSort(int rank, int size){
	int i, j, k, source;
	int aux = N/size;
	int id = 99;
	for(i=0;i<(N/2)+1;i++){
		if(rank != 0){
			MPI_Irecv(&vetor[(rank*aux)-1], 1, MPI_INT, rank-1, id+(rank-1), MPI_COMM_WORLD, &req[0]);
		}
		sortPar(rank);
		
		if(rank == 0){
			MPI_Send(&vetor[aux-1], 1, MPI_INT, 1, id, MPI_COMM_WORLD);
			MPI_Irecv(&vetor[aux-1], 1, MPI_INT, 1, id, MPI_COMM_WORLD, &req[0]);
		}else if(rank == size-1){
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
		}else{
			MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank+1, id+rank, MPI_COMM_WORLD);
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
			MPI_Irecv(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank+1, id+rank, MPI_COMM_WORLD, &req[1]);		
		}
		
		sortImpar(rank);
		
		if(rank == 0){
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
		}else if(rank == size-1){
				MPI_Send(&vetor[(rank*aux)-1], 1, MPI_INT, rank-1, id+(rank-1), MPI_COMM_WORLD);
		}else{
			MPI_Send(&vetor[(rank*aux)-1], 1, MPI_INT, rank-1, id+(rank-1), MPI_COMM_WORLD);
			MPI_Wait(&req[1], MPI_STATUS_IGNORE);
		}
	}
	if(rank == 0){
		for(j=0; j<size-2; j++){
			MPI_Irecv(&vetor[((j+1)*aux)-1], aux, MPI_INT, j+1, id+(j*100), MPI_COMM_WORLD, &req[j]);
		}
		MPI_Irecv(&vetor[((size-1)*aux)-1], aux+1, MPI_INT, (size-1), id+((size-2)*100), MPI_COMM_WORLD, &req[size-2]);
		for(i=0;i<size-1;i++){
			MPI_Waitany(size-1, req, &source, MPI_STATUS_IGNORE);
		}
	}else if(rank == size-1){
		MPI_Send(&vetor[(rank*aux)-1], aux+1, MPI_INT, 0, id+((rank-1)*100), MPI_COMM_WORLD);
	}else{
		MPI_Send(&vetor[(rank*aux)-1], aux, MPI_INT, 0, id+((rank-1)*100), MPI_COMM_WORLD);
	}
}

void leEntrada(){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 200000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int i, rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	N = atoi(argv[1]);
	vetor = malloc(sizeof(int)*N);
	leEntrada();
	vetIniPar = malloc(sizeof(int)*size);
	vetFimPar = malloc(sizeof(int)*size);
	vetFimImpar = malloc(sizeof(int)*size);
	vetIniImpar = malloc(sizeof(int)*size);
		
	defineChunk(size);

	bubbleSort(rank, size);

	MPI_Finalize();
		
	if (rank == 0){
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Execution time\t%f\n", delta);
		#endif
	}
		
	return 0;
}

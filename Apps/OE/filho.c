#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MPI_Comm interCommPai = 0;
MPI_Request request[3];
int N = 0, *vetor = NULL, *vetIndice = NULL, size = 0, indice = 0;

void leEntrada(){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

void sortPar(int rank){
	int aux, i;
	for(i=vetIndice[rank]; i<vetIndice[(rank+size)]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}

}

void sortImpar(int rank){
	int aux, i;
	for(i=vetIndice[(size*2)+rank]; i<vetIndice[(size*3)+rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void bubbleSort(int rank){
	int i;
	int aux = N/size;
	int id = 99;

	if(size == 2){
		for(i=0;i<(N/2)+1;i++){	
			MPI_Irecv(&vetor[aux-1], 1, MPI_INT, 0, id, interCommPai, &request[0]);
			sortPar(1);
			MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			sortImpar(1);
			MPI_Send(&vetor[aux-1], 1, MPI_INT, 0, id, interCommPai);
		}
		MPI_Send(&vetor[aux-1], (N-(aux-1))+1, MPI_INT, 0, id, interCommPai);	
	}
	else{
		for(i=0;i<(N/2)+1;i++){
			if(rank == 0){
				MPI_Irecv(&vetor[((rank+1)*aux)-1], 1, MPI_INT, 0, id, interCommPai, &request[0]);
			}
			else{
				MPI_Irecv(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD, &request[0]);
			}

			sortPar(rank+1);
				
			if(rank == size-2){
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			}else{
				MPI_Send(&vetor[((rank+2)*aux)-1], 1, MPI_INT, rank+1, id+rank+1, MPI_COMM_WORLD);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
				MPI_Irecv(&vetor[((rank+2)*aux)-1], 1, MPI_INT, rank+1, id+rank+1, MPI_COMM_WORLD, &request[1]);
			}

			sortImpar(rank+1);
			
			if(rank == 0){
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, 0, id, interCommPai);
				MPI_Wait(&request[1], MPI_STATUS_IGNORE);
			}else if(rank == size-2){
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD);
			}else{
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD);
				MPI_Wait(&request[1], MPI_STATUS_IGNORE);
			}
		}	
		if(rank == size-2){
			MPI_Send(&vetor[((rank+1)*aux)-1], aux+1, MPI_INT, 0, id+(rank*100), interCommPai);
		}else{
			MPI_Send(&vetor[((rank+1)*aux)-1], aux, MPI_INT, 0, id+(rank*100), interCommPai);
		}
	}
}

int main(int argc, char **argv){

		argv+=1;

		MPI_Init(&argc, &argv);
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_get_parent(&interCommPai);

		N = atoi(argv[2]);
		int numFilhos = atoi(argv[4]);
		size = numFilhos+1;
		vetIndice = malloc(sizeof(int)*size*4);
		vetor = malloc(sizeof(int)*N);
		
		leEntrada();
		
		MPI_Recv(&vetIndice[0], size*4, MPI_INT, 0, 99, interCommPai, MPI_STATUS_IGNORE);
		
		bubbleSort(rank);
		
		MPI_Finalize();
		return 0;
}

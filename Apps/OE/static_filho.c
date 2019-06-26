#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 225000

MPI_Comm interCommPai;
MPI_Request request[3];
int *vetor, *vetIndice, size, indice = 0, rank2;

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

void bubble2(int rank){
		int i;
		for(i=0;i<(N/2)+1;i++){
				MPI_Irecv(&vetor[74999], 1, MPI_INT, 0, 99, interCommPai, &request[0]);
				sortPar(1);
				//MPI_Recv(&vetor[74999], 1, MPI_INT, 0, 99, interCommPai, MPI_STATUS_IGNORE);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
				sortImpar(1);
				MPI_Send(&vetor[74999], 1, MPI_INT, 0, 99, interCommPai);
		}
		MPI_Send(&vetor[74999], N- 74999+1, MPI_INT, 0, 99, interCommPai);		
}

void bubble3(int rank){
		int i, source;
		for(i=0;i<(N/2)+2;i++){
				if(rank == 0){
						MPI_Irecv(&vetor[49999], 1, MPI_INT, 0, 99, interCommPai, &request[0]);
				}else if(rank == 1)
						MPI_Irecv(&vetor[99999], 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &request[0]);

				sortPar(rank+1);
				if(rank == 0){
						MPI_Send(&vetor[99999], 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[99999], 1, MPI_INT, 1, 100, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 1)
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			
				sortImpar(rank+1);
				if(rank == 0){
							MPI_Send(&vetor[49999], 1, MPI_INT, 0, 99, interCommPai);
							MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 1)
						MPI_Send(&vetor[99999], 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
				
		}
		if(rank == 0)
				MPI_Send(&vetor[49999], 50000, MPI_INT, 0, 99, interCommPai);
		else if(rank == 1)
				MPI_Send(&vetor[99999], 50001, MPI_INT, 0, 100, interCommPai);
}

void bubble4(int rank){
		int i, source;
		for(i=0;i<(N/2)+1;i++){

				if(rank == 0){
						MPI_Irecv(&vetor[37499], 1, MPI_INT, 0, 99, interCommPai, &request[0]);
				}else if(rank == 1){
						MPI_Irecv(&vetor[74999], 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 2)
						MPI_Irecv(&vetor[112499], 1, MPI_INT, 1, 101, MPI_COMM_WORLD, &request[0]);

				sortPar(rank+1);

				if(rank == 0){
						MPI_Send(&vetor[74999], 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[74999], 1, MPI_INT, 1, 100, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 1){
						MPI_Send(&vetor[112499], 1, MPI_INT, 2, 101, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[112499], 1, MPI_INT, 2, 101, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 2)
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);


				sortImpar(rank+1);

				if(rank == 0){
						MPI_Send(&vetor[37499], 1, MPI_INT, 0, 99, interCommPai);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 1){
						MPI_Send(&vetor[74999], 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 2){
						MPI_Send(&vetor[112499], 1, MPI_INT, 1, 101, MPI_COMM_WORLD);
				}
		}
		
		if(rank == 0){
				MPI_Send(&vetor[37499], 37500, MPI_INT, 0, 99, interCommPai);
		}else if(rank == 1){
				MPI_Send(&vetor[74999], 37500, MPI_INT, 0, 199, interCommPai);
		}else if(rank == 2){
				MPI_Send(&vetor[112499], 37501, MPI_INT, 0, 299, interCommPai);
		}
}

void bubble8(int rank){
		int i, source;
		for(i=0;i<(N/2)+1;i++){

				if(rank == 0){
						MPI_Irecv(&vetor[18749], 1, MPI_INT, 0, 99, interCommPai, &request[0]);
				}else if(rank == 1){
						MPI_Irecv(&vetor[37499], 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 2){
						MPI_Irecv(&vetor[56249], 1, MPI_INT, 1, 101, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 3){
						MPI_Irecv(&vetor[74999], 1, MPI_INT, 2, 102, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 4){
						MPI_Irecv(&vetor[93749], 1, MPI_INT, 3, 103, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 5){
						MPI_Irecv(&vetor[112499], 1, MPI_INT, 4, 104, MPI_COMM_WORLD, &request[0]);
				}else if(rank == 6){
						MPI_Irecv(&vetor[131249], 1, MPI_INT, 5, 105, MPI_COMM_WORLD, &request[0]);
				}
				

				sortPar(rank+1);

				if(rank == 0){
						MPI_Send(&vetor[37499], 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[37499], 1, MPI_INT, 1, 100, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 1){
						MPI_Send(&vetor[56249], 1, MPI_INT, 2, 101, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[56249], 1, MPI_INT, 2, 101, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 2){
						MPI_Send(&vetor[74999], 1, MPI_INT, 3, 102, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[74999], 1, MPI_INT, 3, 102, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 3){
						MPI_Send(&vetor[93749], 1, MPI_INT, 4, 103, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[93749], 1, MPI_INT, 4, 103, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 4){
						MPI_Send(&vetor[112499], 1, MPI_INT, 5, 104, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[112499], 1, MPI_INT, 5, 104, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 5){
						MPI_Send(&vetor[131249], 1, MPI_INT, 6, 105, MPI_COMM_WORLD);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
						MPI_Irecv(&vetor[131249], 1, MPI_INT, 6, 105, MPI_COMM_WORLD, &request[1]);
				}else if(rank == 6){
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
				}

				sortImpar(rank+1);

				if(rank == 0){
						MPI_Send(&vetor[18749], 1, MPI_INT, 0, 99, interCommPai);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 1){
						MPI_Send(&vetor[37499], 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 2){
						MPI_Send(&vetor[56249], 1, MPI_INT, 1, 101, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 3){
						MPI_Send(&vetor[74999], 1, MPI_INT, 2, 102, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 4){
						MPI_Send(&vetor[93749], 1, MPI_INT, 3, 103, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 5){
						MPI_Send(&vetor[112499], 1, MPI_INT, 4, 104, MPI_COMM_WORLD);
						MPI_Wait(&request[1], MPI_STATUS_IGNORE);
				}else if(rank == 6){
						MPI_Send(&vetor[131249], 1, MPI_INT, 5, 105, MPI_COMM_WORLD);
				}


		}	
		
		if(rank == 0){
				MPI_Send(&vetor[18749], 18750, MPI_INT, 0, 99, interCommPai);
		}else if(rank == 1){
				MPI_Send(&vetor[37499], 18750, MPI_INT, 0, 199, interCommPai);
		}else if(rank == 2){
				MPI_Send(&vetor[56249], 18750, MPI_INT, 0, 299, interCommPai);
		}else if(rank == 3){
				MPI_Send(&vetor[74999], 18750, MPI_INT, 0, 399, interCommPai);
		}else if(rank == 4){
				MPI_Send(&vetor[93749], 18750, MPI_INT, 0, 499, interCommPai);
		}else if(rank == 5){
				MPI_Send(&vetor[112499], 18750, MPI_INT, 0, 599, interCommPai);
		}else if(rank == 6){
				MPI_Send(&vetor[131249], 18751, MPI_INT, 0, 699, interCommPai);
		}
}



int main(int argc, char **argv){
		argv+=1;
		MPI_Init(&argc, &argv);
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_get_parent(&interCommPai);

		int numFilhos = atoi(argv[4]);
		size = numFilhos+1;
		vetIndice = malloc(sizeof(int)*size*4);
		vetor = malloc(sizeof(int)*N);
		leEntrada();

		MPI_Recv(&vetIndice[0], size*4, MPI_INT, 0, 99, interCommPai, MPI_STATUS_IGNORE);


		if(size == 2)
				bubble2(rank);
		else if(size == 3)
				bubble3(rank);
		else if(size == 4)
				bubble4(rank);
		else if(size == 8)
				bubble8(rank);
		MPI_Finalize();

}

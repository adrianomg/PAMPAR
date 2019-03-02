#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int N, *A, *B, *C;

void multiplicaMatriz(int size){
		int i, j, k;
		int max_linha = size;
		for(i=0;i<max_linha;i++){
				for(j=0;j<N;j++){
						for(k=0;k<N;k++){
								C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
						}
				}
		}
}

void preencheMatrizC(int line, int col){
		int i, j;
		for(i=0;i<line;i++){
				for(j=0;j<col;j++){
						C[(i*line)+j] = 0;
				}
		}
}

int main(int argc, char **argv){

	argv+=1;
        MPI_Init(&argc, &argv);
	MPI_Comm intercommPai;
        MPI_Comm_get_parent(&intercommPai);
        N = atoi(argv[2]);
		
	int chunk = atoi(argv[4]);
	//int i;
	//for (i=0;i<5;i++)
	//	printf("%s\n", argv[i]);
	
	A = (int*) malloc(sizeof(int)*(N*chunk));
	C = (int*) malloc(sizeof(int)*(N*chunk));
	B = (int*) malloc(sizeof(int)*N*N);

	MPI_Recv(&A[0], chunk*N, MPI_INT, 0, 99, intercommPai, MPI_STATUS_IGNORE);
	//printf("Filho recebeu matriz A\n");

	MPI_Recv(&B[0], N*N, MPI_INT, 0, 199, intercommPai, MPI_STATUS_IGNORE);
	//printf("Filho recebeu matriz B\n");

	preencheMatrizC(chunk, N);

	multiplicaMatriz(chunk);

	MPI_Send(&C[0], chunk*N, MPI_INT, 0, 99, intercommPai);
	//printf("Filho enviou matriz C para o pai\n");
	MPI_Finalize();
	return 0;
}

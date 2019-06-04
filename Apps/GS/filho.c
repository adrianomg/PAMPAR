#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int N, ini, fim;
float *Q, *Qt;
MPI_Comm interCommPai;

void gramschmidt(){
	int i, j, k;
	float tmp_local=0, tmp_aux=0;
	float tmp=0;
	MPI_Request request;

	for(k=0;k<N;k++){
		tmp = 0;
		tmp_local = 0;
		for(i=ini; i< fim; i++){
			tmp += (Q[(i*N)+k] * Q[(i*N)+k]);
		}
		MPI_Irecv(&tmp_aux, 1, MPI_FLOAT, 0, 99, interCommPai, &request);
		MPI_Send(&tmp,1, MPI_FLOAT, 0, 99, interCommPai);
		MPI_Wait(&request, MPI_STATUS_IGNORE);
		tmp = tmp_aux;
		
		for(i=0; i<N; i++){
			Q[(i*N)+k] /= tmp;
		}

		for(j=k+1; j<N;j++){
			tmp = 0;
			for(i=ini; i < fim; i++){
				tmp += Q[(i*N)+k] * Q[(i*N)+j];
			}
			//It receives tmp_local, sums, and sends to all other processes
			MPI_Irecv(&tmp_aux, 1, MPI_FLOAT, 0, 99, interCommPai, &request);
			MPI_Send(&tmp,1, MPI_FLOAT, 0, 99, interCommPai);
			MPI_Wait(&request, MPI_STATUS_IGNORE);
			tmp = tmp_aux;
			
			for(i=0; i < N; i++){
				Q[(i*N)+j] -= tmp * Q[(i*N)+k];
			}
		}
	}
}

int main(int argc, char **argv){
	argv+=1;
	MPI_Init(&argc, &argv);
	MPI_Comm_get_parent(&interCommPai);
	N = atoi(argv[2]);
	
	ini = atoi(argv[3]);
	fim = atoi(argv[4]);
	Q = malloc(sizeof(float)*N*N);
	
	MPI_Recv(&Q[0], N*N, MPI_FLOAT, 0, 99, interCommPai, MPI_STATUS_IGNORE);

	gramschmidt();

	free(Q);

	MPI_Finalize();
	return 0;
}

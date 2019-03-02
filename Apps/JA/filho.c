#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

MPI_Comm interCommPai;
int N, iter_max = 1000;

void jacobi(int ini, int fim, float *A, float *Anew, float *y){
		int i, j, iter;
		float *aux;
		MPI_Request request[2];
		float pi = 2.0f * asinf(1.0f);
		for(i=0;i<N;i++){
				A[i]   = 0.f;
	            A[((N-1)*N)+i] = 0.f;
	            y[i] = sinf(pi * i / (N-1));
	            A[(i*N)] = y[i];
	            A[(i*N)+N-1] = y[i]*expf(-pi);        
	            Anew[i] = 0.f;
	            Anew[((N-1)*N)+i] = 0.f;
	            Anew[i*N] = y[i];
	            Anew[(i*N)+N-1] = y[i]*expf(-pi);

		}

		for(iter=0;iter<iter_max;iter++){
				for(j=ini; j < fim; j++){
						for(i=1;i<N-1;i++){
								Anew[(j*N)+i] = 0.25f * ( A[(j*N)+i+1] + A[(j*N)+i-1]+ A[((j-1)*N)+i] + A[((j+1)*N)+i]);
						}
				}
				
				MPI_Irecv(&Anew[0], N*N, MPI_FLOAT, 0, 99, interCommPai, &request[0]);
				MPI_Send(&Anew[ini*N], (fim-ini)*N, MPI_FLOAT, 0, 99, interCommPai);
				//MPI_Recv(&Anew[0], N*N, MPI_FLOAT, 0, 99, interCommPai, MPI_STATUS_IGNORE);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		
	 			aux = A;
	 			A = Anew;
	 			Anew = aux;
		}
}

int main(int argc, char **argv){
		argv+=1;
		int pid;
		MPI_Init(&argc, &argv);
		MPI_Comm_get_parent(&interCommPai);
		N = atoi(argv[2]);
		
		float *A, *Anew, *y;
		int *vetFim, *vetIni;
		
		A = malloc(sizeof(float)*N*N);
		Anew = malloc(sizeof(float)*N*N);
		y = malloc(sizeof(float)*N);

		int ini = atoi(argv[3]);
		int fim = atoi(argv[4]);
		jacobi(ini, fim, A, Anew, y);
		MPI_Finalize();
		return 0;
}

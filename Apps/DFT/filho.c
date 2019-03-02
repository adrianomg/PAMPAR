#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
 
long int N;
#define PI2 6.2832

void computaDFT(long int ini, int chunk, float *P, float *Xim, float *Xre, float *x){
	long int k, n;
	int auxK;
	auxK = ini;
	for(k=0; k<chunk; ++k){
		for (n=0 ; n<N ; ++n) 
			Xre[k] += x[n] * cos(n * auxK * PI2 / N);
			for (n=0 ; n<N ; ++n) 
	        	        Xim[k] -= x[n] * sin(n * auxK * PI2 / N);
		P[k] = Xre[k]*Xre[k] + Xim[k]*Xim[k];
		auxK++;
	}
}

int main(int argc, char **argv){
	argv+=1;
	MPI_Init(&argc, &argv);
	MPI_Comm intercommPai;
        MPI_Comm_get_parent(&intercommPai);
        MPI_Request req;
        N = strtol(argv[2], NULL, 10);

        long int inicio = strtol(argv[4], NULL, 10);
        long int fim = strtol(argv[5], NULL, 10);
        int chunk = fim - inicio;
	float *x, *Xre, *Xim, *P;
	long int n;
	x = malloc(sizeof(float)*chunk);
	Xre = malloc(sizeof(float)*N);
	Xim = malloc(sizeof(float)*N);
	P = malloc(sizeof(float)*chunk);
	//printf("N %d \tchunk%d \tinit%ld \tend%ld\n", N, chunk, inicio, fim);
	
	//for(n=0;n<7;n++)
	//	printf("%s\t", argv[n]);
	//printf("\n");
	
	MPI_Irecv(&x[0], chunk, MPI_FLOAT, 0, 99, intercommPai, &req);

	for (n=0 ; n<N ; ++n){
	       	Xre[n] = 0;
	       	Xim[n] = 0;
	}

	MPI_Wait(&req, MPI_STATUS_IGNORE);

	computaDFT(inicio, chunk, P, Xim, Xre, x);
	//MPI_Finalize();
	//return 0;
	MPI_Send(&P[0], chunk, MPI_FLOAT, 0, 99, intercommPai);

        MPI_Finalize();
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#define PI2 6.2832
int N;

void computaDFT(int rank, int *vetIni, int chunk, float *P, float *Xim, float *Xre, float *x){
	int k, n, auxK;
	auxK = vetIni[rank];
	for(k=0; k<chunk; ++k){
	    for (n=0 ; n<N ; ++n) 
            Xre[k] += x[n] * cos(n * auxK * PI2 / N);
	    for (n=0 ; n<N ; ++n) 
            Xim[k] -= x[n] * sin(n * auxK * PI2 / N);
        P[k] = Xre[k]*Xre[k] + Xim[k]*Xim[k];
        auxK++;
    }
}

void ajustaChunk(int *vetIni, int *vetFim, int size){
	int i, j, chunk, chunkPlus, sobra;
	if(N%size == 0){
		chunk = N/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = N/size;
		chunkPlus = chunk+1;
		sobra = N%size;
		for(i=0;i<size;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
			}
		}
	}
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-procs> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 32768\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Request req[128];
	N = atoi(argv[1]);

	int *vetIni, *vetFim, n;
	vetIni = malloc(sizeof(int)*size);
	vetFim = malloc(sizeof(int)*size);
	
	ajustaChunk(vetIni, vetFim, size);
	float *x, *Xre, *Xim, *P;
	int chunk = vetFim[rank] - vetIni[rank];
	int source;
	if(rank == 0){
		x = malloc(sizeof(float)*N);
		Xre = malloc(sizeof(float)*N);
		Xim = malloc(sizeof(float)*N);
		P = malloc(sizeof(float)*N);
		srand(time(0));
	    for (n=0 ; n<N ; ++n){
        	x[n] = ((2.0 * rand()) / RAND_MAX) - 1.0;
        	Xre[n] = 0;
        	Xim[n] = 0;
	    }
	    for(n=1;n<size;n++){
    		MPI_Send(&x[vetIni[n]], vetFim[n] - vetIni[n], MPI_FLOAT, n, 99, MPI_COMM_WORLD);
	    }
		for(n=1;n<size;n++){
			MPI_Irecv(&P[vetIni[n]], vetFim[n] - vetIni[n], MPI_FLOAT, n, 99, MPI_COMM_WORLD, &req[n-1]);
		}
		computaDFT(rank, vetIni, chunk, P, Xim, Xre, x);
		for(n=1;n<size;n++){
			MPI_Waitany(size-1, req, &source, MPI_STATUS_IGNORE);
		}
	}else{
		x = malloc(sizeof(float)*(vetFim[rank]-vetIni[rank]));
		Xre = malloc(sizeof(float)*N);
		Xim = malloc(sizeof(float)*N);
		P = malloc(sizeof(float)*(vetFim[rank]-vetIni[rank]));

		MPI_Irecv(&x[0], vetFim[rank]-vetIni[rank], MPI_FLOAT, 0, 99, MPI_COMM_WORLD, &req[0]);				
		for(n=0;n<chunk;n++){
			Xre[n] = 0;
			Xim[n] = 0;
		}	
		MPI_Wait(&req[0], MPI_STATUS_IGNORE);
		computaDFT(rank, vetIni, chunk, P, Xim, Xre, x);

		MPI_Send(&P[0], chunk, MPI_FLOAT, 0, 99, MPI_COMM_WORLD);
	}
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

long int N;
#define PI2 6.2832

void computaDFT(int rank, long int *vetIni, int chunk, float *P, float *Xim, float *Xre, float *x){
	long int k, n;
	int auxK;
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

void ajustaChunk(long int *vetIni, long int *vetFim, long int size){
	long int i, j;
	int chunk, chunkPlus, sobra;
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

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 32768 \"$PWD/child\"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	int rank, err[1];
	MPI_Init(&argc, &argv);
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	MPI_Request req[128];
	MPI_Comm intercommFilho[128];

	int numFilhos = atoi(argv[1]);
	int size = numFilhos+1;
	N = strtol(argv[2], NULL, 10);

	char *bin;
        int tam1 = strlen(argv[argc-1]);
        bin = (char*)malloc((tam1)*sizeof(char));
        strcpy(bin, argv[argc-1]);

	long int *vetIni, *vetFim, n;
	vetIni = malloc(sizeof(long int)*size);
	vetFim = malloc(sizeof(long int)*size);

	ajustaChunk(vetIni, vetFim, size);
	float *x, *Xre, *Xim, *P;
	int chunk = vetFim[0] - vetIni[0];
	int source = 0;
	//source = malloc(sizeof(int)*numFilhos);
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

	for(n=argc;n<6;n++){
		argv[n] = (char*) malloc(sizeof(char)*10);
	}

	for(n=0;n<numFilhos; n++){
		sprintf(argv[4], "%ld", vetIni[n+1]);
		sprintf(argv[5], "%ld", vetFim[n+1]);
		argv[6] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &intercommFilho[n], err);

		MPI_Send(&x[vetIni[n+1]], vetFim[n+1] - vetIni[n+1], MPI_FLOAT, 0, 99, intercommFilho[n]);

	}

	for(n=0;n<numFilhos;n++){
		MPI_Irecv(&P[vetIni[n+1]], vetFim[n+1] - vetIni[n+1], MPI_FLOAT, 0, 99, intercommFilho[n], &req[n]);
	}

	computaDFT(0, vetIni, chunk, P, Xim, Xre, x);
	for(n=0;n<numFilhos;n++){
		MPI_Waitany(numFilhos, req, &source, MPI_STATUS_IGNORE);
	}

	MPI_Finalize();

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
	return 0;
}


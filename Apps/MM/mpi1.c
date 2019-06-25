#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define ROOT 0

int N, *A, *B, *AA, *C, *CC;

void multiplicaMatriz(int rank, int *vetChunk){
	int i, j, k;
	int max_linha = vetChunk[rank];
	for(i=0;i<max_linha;i++){
		for(j=0;j<N;j++){
			for(k=0;k<N;k++){
				C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
			}
		}
	}
}

void multiplicaMatrizIdeal(int size){
	int i, j, k;
	int max_linha = N/size;
	for(i=0;i<max_linha;i++){
		for(j=0;j<N;j++){
			for(k=0;k<N;k++){
				C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
			}
		}
	}
}

void preencheMatrizAA(int line, int col){
	int i, j;
	for(i=0;i<line;i++){
		for(j=0;j<col;j++){
			AA[(i*line)+j] = i;
		}
	}
}

void preencheMatrizB(int line, int col){
	int i, j;
	for(i=0;i<line;i++){
		for(j=0;j<col;j++){
			B[(i*line)+j] = i+j;
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

void ajustaChunk(int size, int *vetIni, int *vetFim, int *vetChunk){
	int i, j, chunk, chunkPlus, sobra;
	if(N%size == 0){
		chunk = N/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
			vetChunk[i] = chunk;
		}
	}else{
		chunk = N/size;
		chunkPlus = chunk+1;
		sobra = N%size;
		for(i=0;i<size;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
				vetChunk[i] = chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
				vetChunk[i] = chunk;
			}
		}
	}
}

void imprimeMatriz(long int *matriz){
    int i, j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("%ld\t", matriz[(i*N)+j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	N = atoi(argv[1]);
	int *vetIni, *vetFim, *vetChunk, i;

	AA = (int*) malloc(sizeof(int)*N*N);
	CC = (int*) malloc(sizeof(int)*N*N);			
	if(rank == ROOT){
		preencheMatrizAA(N, N);
	}
	A = (int*) malloc(sizeof(int)*(N/size)*N);
	B = (int*) malloc(sizeof(int)*N*N);

	C = (int*) malloc(sizeof(int)*(N/size)*N);
	preencheMatrizC(N/size, N);
	preencheMatrizB(N, N);


	MPI_Scatter(&AA[0], (N/size)*N, MPI_INT, &A[0], (N/size)*N, MPI_INT, ROOT, MPI_COMM_WORLD);
	
	multiplicaMatrizIdeal(size);
	
	MPI_Gather(&C[0], (N/size)*N, MPI_INT, &CC[0], (N/size)*N, MPI_INT, ROOT, MPI_COMM_WORLD);
	
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

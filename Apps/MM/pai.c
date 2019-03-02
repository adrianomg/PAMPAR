#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <sys/time.h>

#define ROOT 0
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

void preencheMatrizA(int line, int col){
	int i, j;
	for(i=0;i<line;i++){
		for(j=0;j<col;j++){
			A[(i*line)+j] = i;
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


int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 2048 "$PWD/child"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
 	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int err[1];
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	
	int num_Proc = atoi(argv[1]);
	MPI_Comm intercommFilho[num_Proc];
 	int size = num_Proc+1;
 	
 	N = atoi(argv[2]);
	char *bin;
    int tam1 = strlen(argv[3]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[3]);

	int i, *vetChunk, *vetIni, *vetFim;

	A = (int*) malloc(sizeof(int)*N*N);
	B = (int*) malloc(sizeof(int)*N*N);
	C = (int*) malloc(sizeof(int)*N*N);			
	vetChunk = malloc(sizeof(int)*size);
	vetIni = malloc(sizeof(int)*size);
	vetFim = malloc(sizeof(int)*size);
	preencheMatrizA(N, N);
	preencheMatrizB(N, N);
	preencheMatrizC(N, N);

	ajustaChunk(size, vetIni, vetFim, vetChunk);

	for(i=argc;i<8;i++){
		argv[i] = (char*) malloc(sizeof(char)*10);
	}
	for(i=0;i<num_Proc;i++){
		sprintf(argv[4], "%d", vetChunk[i+1]);
		argv[5] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, rank, MPI_COMM_SELF, &intercommFilho[i], err);
		MPI_Send(&A[vetIni[i+1]], vetChunk[i+1], MPI_INT, 0, 99, intercommFilho[i]);		
		MPI_Send(&B[0], N*N, MPI_INT, 0, 199, intercommFilho[i]);
	}
	multiplicaMatriz(vetChunk[rank]);

	for(i=0;i<num_Proc;i++){
		MPI_Recv(&C[vetIni[i+1]*N], vetChunk[i+1]*N, MPI_INT, 0, 99, intercommFilho[i], MPI_STATUS_IGNORE);
	}

	MPI_Finalize();
        
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
	
	return 0;
}

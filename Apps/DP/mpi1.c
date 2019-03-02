#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

long long int N;

void ajustaChunk(int size, long long int *vetIni, long long int *vetFim){
	long long int i, j, chunk, chunkPlus, sobra;
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
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 10000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
		
	N = strtoul(argv[1], NULL, 10);
	long long int *vetIni, *vetFim;
	vetIni = malloc(sizeof(long long int)*size);
	vetFim = malloc(sizeof(long long int)*size);
	ajustaChunk(size, vetIni, vetFim);
	long long int ini = vetIni[rank], fim = vetFim[rank];
	long long int i;
	long long int sum_local=0, sum_final=0;

	for(i=ini;i<fim;i++){
		sum_local += i * (N-i);
	}

	MPI_Reduce(&sum_local, &sum_final, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	//if(rank == 0)
	//	printf("%llu\n", sum_final);

	MPI_Finalize();
        
	if (rank == 0){
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Excution time\t%f\n", delta);
		#endif
	}
	return 0;
}

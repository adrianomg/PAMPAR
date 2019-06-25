#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

long long int n;

double f (double x){
    double pi = 3.141592653589793;
    double value;
    value = 50.0 / (pi * (2500.0 * x * x + 1.0));
    return value;
}

void ajustaChunk(int size, long long int *vetIni, long long int *vetFim){
	long long int i, j, chunk, chunkPlus, sobra;
	if(n%size == 0){
		chunk = n/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = n/size;
		chunkPlus = chunk+1;
		sobra = n%size;
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
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 1000000000\n\n");
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
	
	n = strtoul(argv[1], NULL, 10);
	
	long long int *vetIni, *vetFim;
	vetIni = malloc(sizeof(long long int)*size);
	vetFim = malloc(sizeof(long long int)*size);
	ajustaChunk(size, vetIni, vetFim);
	
	int ini = vetIni[rank], fim = vetFim[rank];
	int i;
	int sum_local=0, sum_final=0;
	double a=0.0, b = 10.0;
	double total=0.0, x, total_global=0;

	for(i=ini;i<fim;i++){
		x = ((n-i-1)*a+(i)*b)/(n-1);
		total = total + f(x);
	}

	MPI_Reduce(&total, &total_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		total_global = (b-a)*total_global/(double)n;
	//	printf("%f\n", total_global);
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

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define ROOT 0
long long int NUM_PONTOS;

float calculaPi(int size){
    long long int j, chunk;
    float pi=0.0;
    chunk = NUM_PONTOS/size;
    for(j=0;j<chunk;j++){
        pi += 4.0/(4.0*j+1.0);
        pi -= 4.0/(4.0*j+3.0);
    }
    return pi;
}

int main(int argc, char **argv){
	
	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 4000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	int size, rank;
	float pi_final, pi;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	NUM_PONTOS = strtoul(argv[1], NULL, 10);
	
	pi = calculaPi(size);
	MPI_Reduce(&pi, &pi_final, 1, MPI_FLOAT, MPI_SUM, ROOT, MPI_COMM_WORLD);

	if(rank == ROOT){
		pi_final = pi_final/size;
	}

	MPI_Finalize();
        
	if (rank == ROOT){
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Excution time\t%f\n", delta);
		#endif
	}

	if(rank == ROOT)
		printf("%lf\n", pi_final);
	
	return 0;
}


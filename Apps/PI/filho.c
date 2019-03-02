#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

MPI_Comm comm_pai;
long long int NUM_PONTOS;
int rank2, indice;

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
	
	int pid, nFilhos;
	MPI_Init(&argc, &argv);
	MPI_Comm_get_parent(&comm_pai);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &nFilhos);
        rank2=pid+1;

	
	NUM_PONTOS = strtoul(argv[3], NULL, 10);
	nFilhos = atoi(argv[2]);

	float pi = 0.0;
	pi = calculaPi(nFilhos+1);

	MPI_Send(&pi, 1, MPI_FLOAT, 0, 99, comm_pai);

	MPI_Finalize();
	return 0;
}

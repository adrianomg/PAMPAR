#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

long long int N;

int main(int argc, char **argv){
	argv+=1;
	
	MPI_Init(&argc, &argv);
	MPI_Comm intercommPai;
	MPI_Comm_get_parent(&intercommPai);
	N = strtoul(argv[2], NULL, 10);

	long long int ini = atol(argv[3]);
	long long int fim = atol(argv[4]);
	long long int sum=0, i;

	for(i=ini;i<fim;i++)
		sum += i*(N-i);
	
	MPI_Send(&sum, 1, MPI_LONG, 0, 99, intercommPai);
	 
	MPI_Finalize();
	
	return 0;
}

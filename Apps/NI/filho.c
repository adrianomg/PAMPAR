#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

long long int n;

double f (double x){
        double pi = 3.141592653589793;
        double value;
        value = 50.0 / (pi * (2500.0 * x * x + 1.0));
        return value;
}

int main(int argc, char **argv){
	argv+=1;
	MPI_Init(&argc, &argv);
	MPI_Comm intercommPai;
	MPI_Comm_get_parent(&intercommPai);
	n = strtoul(argv[2], NULL, 10);
	
	long long int ini = atol(argv[4]);
	long long int fim = atol(argv[5]);

	//printf("n = %lld ini = %lld fim = %lld \n", n, ini, fim);

	double total=0, x, a=0,b=10.0;
	long long int i;
	for(i=ini;i<fim;i++){
		x = ((n-i-1)*a+(i)*b)/(n-1);
		total = total +f(x);
	}

	MPI_Send(&total, 1, MPI_DOUBLE, 0, 99, intercommPai);
	MPI_Finalize();
	return 0;
}

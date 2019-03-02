#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

#define MASTER 0

using namespace std;

void sum(char* output, const long unsigned int d, const long unsigned int n, int rank, int size) {

	long unsigned int inicio[size], fim[size], qtdLinhas;
	long unsigned int digits[d + 11], digitsOp[d+11];
		
	for(long unsigned int i=0; i < size; i++){
		inicio[i] = 0;
		fim[i] = 0;
	}
	for (long unsigned int digit = 0; digit < d + 11; ++digit) {
		digits[digit] = 0;
		digitsOp[digit] = 0;
	}

	if(n%size == 0){
		inicio[0] = 1;
		fim[0] = n/size;
		for(long unsigned int i=1;i<size;i++){
			inicio[i] = ((n/size) * i) + 1;
			fim[i] = (n/size)*(i+1);
		}
	}else{
		long unsigned int sobra = n%size, j=0;
		inicio[0] = 1;
		fim[0] = n/size + 1;
		sobra = sobra - 1;	
		
		for(long unsigned int i=1;i<=size;i++){
			if(i <= sobra){
				inicio[i] = (n/size)*(i) + 2;
				fim[i] = (n/size)*(i+1) + 2;
			}else{
				inicio[i] = (n/size)*(i) + i+1;
				fim[i] = (n/size)*(i+1) + i+1 - j;
				j++;
			}
		}
	 }

	for (long unsigned int i = inicio[rank]; i <= fim[rank]; ++i) {
		long unsigned int remainder = 1;
		/*Cálculo do resto da divisão*/
		for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
			long unsigned int div = remainder / i;
			long unsigned int mod = remainder % i;
			digitsOp[digit] += div;
			remainder = mod * 10;
		}
	}
	long unsigned int tamanho = d+11;
	MPI::COMM_WORLD.Reduce(&digitsOp, &digits, tamanho, MPI::UNSIGNED_LONG, MPI::SUM, MASTER);

	if(rank == MASTER){
		for (long unsigned int i = d + 11 - 1; i > 0; --i) {
			digits[i - 1] += digits[i] / 10;
			digits[i] %= 10;
		}
		if (digits[d + 1] >= 5) {
			++digits[d];
		}
		for (long unsigned int i = d; i > 0; --i) {
			digits[i - 1] += digits[i] / 10;
			digits[i] %= 10;
		}
		stringstream stringstreamA;
		stringstreamA << digits[0] << ",";
		for (long unsigned int i = 1; i <= d; ++i) {
			stringstreamA << digits[i];
		}
		string stringA = stringstreamA.str();
		stringA.copy(output, stringA.size());
	}
}

int main(int argc, char **argv) {

	if (argc < 3) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <iterations> <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 100000 100000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int rank, size;
	MPI::Init(argc, argv);
	rank = MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();


	long unsigned int d, n;
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	char output[d + 10]; // extra precision due to possible error
	
	sum(output, d, n, rank, size);

	MPI::Finalize();
	
	if (rank == 0){
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Excution time\t%f\n", delta);
		#endif
    }
	return 0;
}

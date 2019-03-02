#include <iostream>
#include <sstream>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
 
using namespace std;
MPI_Comm comm_pai;
long unsigned int d;
 
void sum(int inicio, int fim){
 
        long unsigned int digits[d+11];    
        for (long unsigned int digit = 0; digit < d + 11; ++digit) {
                digits[digit] = 0;
        }
       
        for (long unsigned int i = inicio; i <= fim; ++i) {
                long unsigned int remainder = 1;
                /*Cálculo do resto da divisão*/
                for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
                       
                    long unsigned int div = remainder / i;
                    long unsigned int mod = remainder % i;
                    digits[digit] += div;
                    remainder = mod * 10;
                }
        }
 	
        MPI_Send(&digits[0], (fim-inicio), MPI_UNSIGNED_LONG, 0, 99, comm_pai);    
}
 
 
int main(int argc, char **argv){
	argv+=1;
        int inicio, fim;
 
        MPI_Init(&argc, &argv);
        MPI_Comm_get_parent(&comm_pai);
	
	d = (long unsigned int) atoi(argv[2]);
        inicio = atoi(argv[3]);
        fim = atoi(argv[4]);
	
	//printf("%lu %d %d\n", d, inicio, fim);

        sum(inicio, fim);
 
        MPI_Finalize();
 
        return 0;
 
}

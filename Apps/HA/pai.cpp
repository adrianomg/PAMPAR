#include <iostream>
#include <sstream>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

using namespace std;
long unsigned int d, n;
int inicio[128], fim[128];
MPI_Comm interCommFilho[128];
 
void ajusta(int size){
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
}
 
void sum(char *output, int nFilhos){
    long unsigned int dig_local[d+11], digits[d+11];       
    for (long unsigned int digit = 0; digit < d + 11; ++digit) {
        digits[digit] = 0;
    }

    for (long unsigned int i = inicio[0]; i <= fim[0]; ++i) {
        long unsigned int remainder = 1;
        /*Cálculo do resto da divisão*/
        for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
            long unsigned int div = remainder / i;
            long unsigned int mod = remainder % i;
            digits[digit] += div;
            remainder = mod * 10;
        }
    }

    for(long unsigned int i=0;i<nFilhos;i++){
            
		MPI_Recv(&dig_local[0], (fim[i+1]-inicio[i+1]), MPI_UNSIGNED_LONG, 0, 99, interCommFilho[i], MPI_STATUS_IGNORE);

        for(long unsigned int i=0;i<d+11;i++){
            digits[i] += dig_local[i];
        }
    }

    for (long unsigned int i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }
    if (digits[d + 1] >= 5) {
        ++digits[d];
    }
    for (long unsigned int i = d; i > 0; --i){
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
 
int main(int argc, char **argv){

	if (argc < 5) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <iterations> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 100000 100000 \"$PWD/child\"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    MPI_Init(&argc, &argv);
    MPI_Info localInfo;
    MPI_Info_create(&localInfo);

    int nFilhos = atoi(argv[1]);
    int err[1];
    int i;

    d = (long unsigned int) atoi(argv[2]);
    n = (long unsigned int) atoi(argv[3]);
    
    char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);

       
    argv[2] = (char*)malloc(sizeof(char)*10);
    argv[3] = (char*)malloc(sizeof(char)*10);
    argv[4] = (char*)malloc(sizeof(char)*10);

    char output[d + 10];
   
    ajusta(nFilhos+1);
   
    for(i=0;i<nFilhos;i++){
		sprintf(argv[2], "%lu", d);
        sprintf(argv[3], "%d", inicio[i+1]);
        sprintf(argv[4], "%d", fim[i+1]);
        MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &interCommFilho[i], err);
    }
   
    sum(output, nFilhos);
   
    MPI_Finalize();
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

    return 0;
}

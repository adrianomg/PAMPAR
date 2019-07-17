#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int *A, *B;
long int *C;

void multiplicaMatriz(int N){
	int i, j, k;
	for(i=0;i<N;i++){
		for(k=0;k<N;k++){
			for(j=0;j<N;j++){
				C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
			}
		}
	}
}

void preencheMatriz(int N){
	int i, j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			A[(i*N)+j] = i;
			B[(i*N)+j] = i+j;
			C[(i*N)+j] = 0;
		}
	}
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
        
	int N = atoi(argv[1]);

	A = malloc(sizeof(int)*N*N);
	B = malloc(sizeof(int)*N*N);
	C = malloc(sizeof(long int)*N*N);

	preencheMatriz(N);

	multiplicaMatriz(N);
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

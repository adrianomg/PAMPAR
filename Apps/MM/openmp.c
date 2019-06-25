#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int indice, N, *A, *B;
long int *C;

void multiplicaMatriz(){
	int i, j, k;
	#pragma omp parallel private (i, j, k)
	{
		#pragma omp for schedule(static)
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				for(k=0;k<N;k++){
					C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
				}
			}
		}
	}
}

void imprimeMatriz(int *matriz){
	int i, j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			printf("%d\t", matriz[(i*N)+j]);
		}
		printf("\n");
	}
}
void preencheMatriz(){
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
	
	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	N = atoi(argv[2]);
	int num_threads = atoi(argv[1]);
	omp_set_num_threads(num_threads);		
	A = malloc(sizeof(int)*N*N);
	B = malloc(sizeof(int)*N*N);
	C = malloc(sizeof(long int)*N*N);

	preencheMatriz();
	multiplicaMatriz();

	/*imprimeMatriz(A);
	imprimeMatriz(B);
		imprimeMatriz(C);*/
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

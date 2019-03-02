//Code for access memory: sort 1000 elements
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int nt, indice, N;

void sortParImpar(int *vetor){
	int i, k, aux;
	long long valor = 0;

	#pragma omp parallel private(k, i, aux, valor)
	{
		for(k=0;k<(N/2);k++){
			#pragma omp for schedule(static)
			for(i=0;i<N-1;i = i+2){
				if(vetor[i] > vetor[i+1]){
					aux = vetor[i];
					vetor[i] = vetor[i+1];
					vetor[i+1] = aux;
				}
			}
			#pragma omp for schedule(static)
			for(i=1;i<N-1;i=i+2){
				if(vetor[i] > vetor[i+1]){
						aux = vetor[i];
						vetor[i] = vetor[i+1];
						vetor[i+1] = aux;
				}
			}
		}
	}
}

void leEntrada(int *vetor){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 200000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int i;
	omp_set_num_threads(atoi(argv[1]));
	N = atoi(argv[2]);
	int *vetor = malloc(sizeof(int)*N);
	leEntrada(vetor);
	sortParImpar(vetor);
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
void sortParImpar(int *vetor, int N){
	int i, k;
	int aux;
	for(k=0;k<N/2;k++){
		for(i=0;i<N-1;i=i+2){
			if(vetor[i] > vetor[i+1]){
				aux = vetor[i];
				vetor[i] = vetor[i+1];
				vetor[i+1] = aux;
			}
		}
		for(i=1;i<N-1;i=i+2){
			if(vetor[i] > vetor[i+1]){
				aux = vetor[i];
				vetor[i] = vetor[i+1];
				vetor[i+1] = aux;
			}
		}
	}
}

void leEntrada(int *vetor, int N){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 200000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int i, *vetor, N;
	N = atoi(argv[1]);
	vetor = malloc(sizeof(int)*N);
	leEntrada(vetor, N);
	
	sortParImpar(vetor, N);
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
}

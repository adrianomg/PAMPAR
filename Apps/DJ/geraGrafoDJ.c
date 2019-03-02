#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){
	FILE *output;
	int N, i, j, valor, *matriz;
	srand(time(NULL));
	N = atoi(argv[1]);
	output = fopen(argv[2], "w");
	matriz = malloc(sizeof(int)*N*N);

	for(i=0;i<N;i++){
		for(j=i;j<N;j++){
			if(i==j){
					matriz[(i*N)+j] = 0;
			}else{
					valor = rand()%100;
					if(valor < 30 || valor > 90){
							matriz[(i*N)+j] = -1;
					}else{
							matriz[(j*N)+i] = valor;
					}
			}
		}
	}
	for(i=0;i<N;i++){
			for(j=0;j<N;j++){
					fprintf(output, "%d ", matriz[(i*N)+j]);
			}
			fprintf(output, "\n");
	}
	return 0;
}

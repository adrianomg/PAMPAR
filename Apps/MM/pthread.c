#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int num_threads, N, *A, *B, *vetIni, *vetFim, indice;
long int *C;

void multiplicaMatriz(long int idThread){
	int i, j, k;
	for(i=vetIni[idThread-1];i<vetFim[idThread-1];i++){
		for(k=0;k<N;k++){
			for(j=0;j<N;j++){
				C[(i*N)+j] += A[(i*N)+k] * B[(k*N)+j];
			}
		}
	}
}

void *matrixMultiplication(void *threadId){
	long int idThread = (long int) threadId;
	multiplicaMatriz(idThread);
	return NULL;
}


void ajustaChunk(){
	int i, j, chunk, chunkPlus, sobra;
	if(N%num_threads == 0){
		chunk = N/num_threads;
		for(i=0;i<num_threads;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = N/num_threads;
		chunkPlus = chunk+1;
		sobra = N%num_threads;
		for(i=0;i<num_threads;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
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
	num_threads = atoi(argv[1]);
	long int i;
	pthread_t threads[num_threads];
	A = malloc(sizeof(int)*N*N);
	B = malloc(sizeof(int)*N*N);
	C = malloc(sizeof(long int)*N*N);
	vetIni = malloc(sizeof(int)*num_threads);
	vetFim = malloc(sizeof(int)*num_threads);

	preencheMatriz();

	ajustaChunk();

	for(i=1;i<=num_threads;i++){
		pthread_create(&threads[i], NULL, matrixMultiplication, (void *)i);
	}

	for(i=1;i<=num_threads;i++){
		pthread_join(threads[i], NULL);
	}		
	
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

//Code for access memory: sort 1000 elements
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

pthread_barrier_t cond;
pthread_mutex_t mutexSum;
long int numThreads;
int N, indice, *vetor, *vetIniImpar, *vetFimImpar, *vetIniPar, *vetFimPar;

void sortPar(int idThread){
	int aux, i;
	for(i=vetIniPar[idThread]; i<vetFimPar[idThread]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void sortImpar(int idThread){
	int aux, i;
	for(i=vetIniImpar[idThread]; i<vetFimImpar[idThread]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}


void *sortParImpar(void *id){
		
	int idThread = (long int) id;
	int i;
	for(i=0;i<N/2;i++){
		sortPar(idThread);
		pthread_barrier_wait(&cond);
		sortImpar(idThread);
		pthread_barrier_wait(&cond);
	}
	return NULL;
}


void ajustaChunkPar(){
	int i, TrProc, TrProcPlus, nThreads, nTrocas, salto, sobra;
	nTrocas = N/2;
	if(nTrocas%numThreads == 0){
		TrProc = nTrocas/numThreads;
		salto = TrProc*2;
		for(i=0;i<numThreads;i++){
			vetIniPar[i] = i*TrProc*2;
			vetFimPar[i] = vetIniPar[i]+salto;
		}

	}else{
		TrProc = nTrocas/numThreads;
		TrProcPlus = TrProc+1;
		sobra = nTrocas%numThreads;
		for(i=0;i<numThreads;i++){
			if(i<sobra){
				vetIniPar[i] = i*TrProcPlus*2;
				vetFimPar[i] = vetIniPar[i]+(TrProcPlus*2);
			}else{
				vetIniPar[i] = vetFimPar[i-1];
				vetFimPar[i] = vetIniPar[i]+(TrProc*2);
			}
		}
	}
}

void ajustaChunkImpar(){
	int i, TrProc, TrProcPlus, nThreads, nTrocas, salto, sobra;
	if(N%2 == 0){
		nTrocas = (N/2)-1;
	}else{
		nTrocas = (N/2);
	}
	if(nTrocas == 0)
		nTrocas = 1;

	if(nTrocas%numThreads == 0 && nTrocas > 1){
		TrProc = nTrocas/numThreads;
		salto = TrProc*2;
		for(i=0;i<numThreads;i++){
			vetIniImpar[i] = (i*TrProc*2)+1;
			vetFimImpar[i] = vetIniImpar[i]+salto;
		}
	}else{
		TrProc = nTrocas/numThreads;
		TrProcPlus = TrProc+1;
		sobra = nTrocas%numThreads;
		for(i=0;i<numThreads;i++){
			if(i<sobra){
				vetIniImpar[i] = (i*TrProcPlus*2)+1;
				vetFimImpar[i] = vetIniImpar[i] + (TrProcPlus*2);
			}else{
				vetIniImpar[i] = vetFimImpar[i-1];
				vetFimImpar[i] = vetIniImpar[i] + (TrProc*2);
			}
		}
	}
}


void leEntrada(){
	int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 4000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	long int i;
	numThreads = atol(argv[1]);
	N = atoi(argv[2]);
	vetor = malloc(sizeof(int)*N);
	leEntrada();
	pthread_t threads[numThreads];
	pthread_barrier_init(&cond, NULL, numThreads);
	vetIniPar = malloc(sizeof(int)*numThreads);
	vetFimPar = malloc(sizeof(int)*numThreads);
	vetFimImpar = malloc(sizeof(int)*numThreads);
	vetIniImpar = malloc(sizeof(int)*numThreads);
	
	ajustaChunkPar();
	ajustaChunkImpar();

	for(i=0;i<numThreads;i++){
			pthread_create(&threads[i], NULL, sortParImpar, (void *)i);
	}
	for(i=0;i<numThreads;i++){
			pthread_join(threads[i], NULL);
	}
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

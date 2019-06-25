#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

long long int N;
long int numThreads;
long long int *vetIni, *vetFim;
pthread_mutex_t mutex_sum;
long long int sum_total =0;

void *calculaDot(void *threadid){
	long long int i;
	long int idThread = (long int) threadid;
	long long int sum_private=0;
	long long int ini = vetIni[idThread], fim = vetFim[idThread];
	long long int cont=0;
	for(i=ini; i<fim; i++){
		sum_private += i * (N-i);
	}
	pthread_mutex_lock(&mutex_sum);
		sum_total += sum_private;
	pthread_mutex_unlock(&mutex_sum);
	return NULL;
}

void ajustaChunk(){
	long long int i, j, chunk, chunkPlus, sobra;
	if(N%numThreads == 0){
		chunk = N/numThreads;
		for(i=0;i<numThreads;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = N/numThreads;
		chunkPlus = chunk+1;
		sobra = N%numThreads;
		for(i=0;i<numThreads;i++){
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

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 10000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	long int i;
	N = strtoul(argv[2], NULL, 10);
	numThreads = atoi(argv[1]);
	pthread_t threads[numThreads];
	vetIni = malloc(sizeof(long long int)*numThreads);
	vetFim = malloc(sizeof(long long int)*numThreads);
	ajustaChunk();
	for(i=0;i<numThreads;i++)
		pthread_create(&threads[i], NULL, calculaDot, (void*)i);
	for(i=0;i<numThreads;i++)
		pthread_join(threads[i], NULL);
	//printf("%llu\n", sum_total);

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

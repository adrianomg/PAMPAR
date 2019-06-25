#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef PAPI
        #include <papi.h>
#endif


long long int NUM_PONTOS;
long int numThreads;
float pi_final=0.0;
pthread_mutex_t mutexSum;
int indice;
long long total_all=0;

void *calculaPi(void *threadid){

	long long int j, chunk;
	//long int idThread;
	float pi=0.0;
	//idThread = (long int)threadid;
	chunk = NUM_PONTOS/numThreads;
	for(j=0;j<chunk;j++){
		pi += 4.0/(4.0*j+1.0);
		pi -= 4.0/(4.0*j+3.0);
	}
	pthread_mutex_lock(&mutexSum);
	pi_final += pi;
	pthread_mutex_unlock(&mutexSum);

	return NULL;
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 4000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	NUM_PONTOS = strtoul(argv[2], NULL, 10);
	long int i;
	numThreads = atol(argv[1]);
	pthread_t threads[numThreads];
	//printf("%lld\n", NUM_PONTOS);
	for(i=1;i<=numThreads;i++){
		pthread_create(&threads[i], NULL, calculaPi, (void *)i);
	}
	for(i=1;i<=numThreads;i++){
		pthread_join(threads[i], NULL);
	}
	pi_final = pi_final/numThreads;
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	printf("Pi = %lf\n", pi_final);	
	return 0;

}


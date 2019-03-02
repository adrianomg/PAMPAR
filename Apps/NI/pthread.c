#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

long long int n;
long int numThreads;
long long int *vetIni, *vetFim;
pthread_mutex_t mutex_sum;
double total_global = 0;
double a = 0.0, b = 10.0;
pthread_mutex_t mutexSum;

double f(double x){
	double pi = 3.141592653589793;
    double value;
    value = 50.0 / ( pi * ( 2500.0 * x * x + 1.0 ) );
    return value;
}

void *calculaquad(void *threadid){

	long int idThread = (long int) threadid;
	long long int ini = vetIni[idThread], fim = vetFim[idThread];
	int i;
	double x, total = 0.0;
	for(i=ini; i<fim; i++){
		x = ((n-i-1)*a+(i)*b)/(n-1);
		total = total +f(x);
	}
	pthread_mutex_lock(&mutex_sum);
	total_global += total;
	pthread_mutex_unlock(&mutex_sum);
	return NULL;
}

void ajustaChunk(){
	long long int i, j, chunk, chunkPlus, sobra;
	if(n%numThreads == 0){
		chunk = n/numThreads;
		for(i=0;i<numThreads;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = n/numThreads;
		chunkPlus = chunk+1;
		sobra = n%numThreads;
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
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 1000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	long int i;
	n = strtoul(argv[2], NULL, 10);
	numThreads = atoi(argv[1]);
	pthread_t threads[numThreads];
	vetIni = malloc(sizeof(long long int)*numThreads);
	vetFim = malloc(sizeof(long long int)*numThreads);
	ajustaChunk();

	for(i=0;i<numThreads;i++)
			pthread_create(&threads[i], NULL, calculaquad, (void*)i);
	for(i=0;i<numThreads;i++)
			pthread_join(threads[i], NULL);

	total_global = (b-a)*total_global/(double)n;
	//printf("%f\n", total_global);
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
}

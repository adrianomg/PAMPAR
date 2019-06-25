#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define PI2 6.2832

long int numThreads;
float *x, *Xre, *Xim, *P;
long int *vetIni, *vetFim;
int N, indice;
pthread_mutex_t mutexSum;
long long total_all=0;

void computaDFT(long int idThread){
	int k, n;
	for(k=vetIni[idThread-1] ; k<vetFim[idThread-1]; ++k){
	    for (n=0 ; n<N ; ++n) 
            Xre[k] += x[n] * cos(n * k * PI2 / N);
	    for (n=0 ; n<N ; ++n) 
            Xim[k] -= x[n] * sin(n * k * PI2 / N);
        P[k] = Xre[k]*Xre[k] + Xim[k]*Xim[k];
    }
}

void *calculaDFT(void *idThread){
	long int id = (long int) idThread;
	computaDFT(id);
	return NULL;
}

void ajustaChunk(){
	int i, j, chunk, chunkPlus, sobra;
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
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 32768\n\n");
		exit(1);
	}
	
	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

    long int n; 
    int k;
    N = atoi(argv[2]);
    x = malloc(sizeof(float)*N);		// discrete-time signal, x
    Xre = malloc(sizeof(float)*N); 
    Xim = malloc(sizeof(float)*N);		// DFT of x (real and imaginary parts)
    P = malloc(sizeof(float)*N);		// power spectrum of x

    srand(time(0));
    for (n=0 ; n<N ; ++n){
    	x[n] = ((2.0 * rand()) / RAND_MAX) - 1.0;
    	Xre[n] = 0;
    	Xim[n] = 0;
    }
    
    numThreads = atoi(argv[1]);
    pthread_t threads[numThreads];
    vetFim = malloc(sizeof(long int)*numThreads);
    vetIni = malloc(sizeof(long int)*numThreads);
    ajustaChunk();

    for(n=1;n<=numThreads;n++)
		pthread_create(&threads[n], NULL, calculaDFT, (void*)n);
    for(n=1;n<=numThreads;n++)
		pthread_join(threads[n], NULL);

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

    return 0;
}

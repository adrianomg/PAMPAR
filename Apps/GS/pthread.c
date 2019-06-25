#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

int indice, *vetIni, *vetFim;
float *Q, tmp=0;
long int numThread;
int N;
pthread_barrier_t cond, condInicio;
pthread_mutex_t mutexSum;

void *grannSchmidt(void *id){
	int i, j, k;
	long int idThread = (long int) id;
	float tmp_local;
	pthread_barrier_wait(&condInicio);
	for(k=0; k < N; k++) {
		tmp = 0.;
		tmp_local=0;
		for(i=vetIni[idThread]; i < vetFim[idThread]; i++) 
			tmp_local +=  (Q[(i*N)+k] * Q[(i*N)+k]);

		//REALIZAR SOMA DE TMP
		pthread_barrier_wait(&condInicio);
		pthread_mutex_lock(&mutexSum);
				tmp += tmp_local;
		pthread_mutex_unlock(&mutexSum);
		pthread_barrier_wait(&cond);
		
		if(idThread == 0)
			tmp = sqrt(tmp);

		pthread_barrier_wait(&cond);

		for(i=vetIni[idThread]; i < vetFim[idThread]; i++) 
			Q[(i*N)+k] /= tmp;

		pthread_barrier_wait(&cond);

		for(j=k+1; j < N; j++) {
			tmp=0.;
			tmp_local=0;
			for(i=vetIni[idThread]; i < vetFim[idThread]; i++) 
				tmp_local += Q[(i*N)+k] * Q[(i*N)+j];

			//REALIZAR SOMA DE TMP
			pthread_barrier_wait(&condInicio);
			pthread_mutex_lock(&mutexSum);
			tmp += tmp_local;
			pthread_mutex_unlock(&mutexSum);
			pthread_barrier_wait(&cond);

			for(i=vetIni[idThread]; i < vetFim[idThread]; i++) 
				Q[(i*N)+j] -= tmp * Q[(i*N)+k];
			pthread_barrier_wait(&condInicio);
		}
		pthread_barrier_wait(&cond);
	}
	return NULL;
}


void checkOctave(){
	int found_error=0;
	int c1, c2, i;
	float sum;
	for(c1=0; c1 < N; c1++){
		for(c2=c1; c2 < N; c2++){
			sum=0.;
			for(i=0; i < N; i++) 
				sum += Q[(i*N)+c1] * Q[(i*N)+c2];
			if(c1 == c2) { // should be near 1 (unit length)
				if(sum < 0.9) {
					printf("Failed unit length: %d %d %g\n", c1,c2,sum);
					found_error = 1;
					exit(1);
				}
				}else{ // should be very small (orthogonal)
					if(sum > 0.1) {
						printf("Failed orthonogonal  %d %d %g\n", c1,c2,sum);
						found_error = 1;
						exit(1);
				}
			}
		}
	}
	if(!found_error) printf("Check OK!\n");
}

void ajustaChunk(){
	int i, j, chunk, chunkPlus, sobra;
	if(N%numThread == 0){
		chunk = N/numThread;
		for(i=0;i<numThread;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = N/numThread;
		chunkPlus = chunk+1;
		sobra = N%numThread;
		for(i=0;i<numThread;i++){
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


int main(int argc, char *argv[]){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	
	numThread = atol(argv[1]);
	pthread_barrier_init(&cond, NULL, numThread);
	pthread_barrier_init(&condInicio, NULL, numThread);
	pthread_mutex_init(&mutexSum, NULL);
	pthread_t threads[numThread];

	long int i;
	int j;
	N = atoi(argv[2]);

	Q = malloc(sizeof(float)*N*N);
	vetIni = malloc(sizeof(int)*numThread);
	vetFim = malloc(sizeof(int)*numThread);
	
	// fill matrix A with random numbers
	for(i=0; i < N; i++)
		for(j=0; j < N; j++)
			Q[(i*N)+j] = (float)rand()/ (float)RAND_MAX;

	//printOctave("A", A, N, N);
	ajustaChunk();

	for(i=0;i<numThread;i++){
		pthread_create(&threads[i], NULL, grannSchmidt, (void *) i);
	}        
	for(i=0;i<numThread;i++){
		pthread_join(threads[i], NULL);
	}
	//checkOctave();
	
	free(Q);
	

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

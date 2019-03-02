#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

long int num_threads;
int N, iter_max=1000, indice, *vetFim, *vetIni, *vetFim2, *vetIni2;
float *A, *Anew, *aux, pi, *yO;

pthread_barrier_t cond;
pthread_mutex_t ajuste;

void ajustaChunk1(){
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

void ajustaChunk2(){
    int i, j, chunk, chunkPlus, sobra;
    if( (N-2)%num_threads == 0){
        chunk = (N-2)/num_threads;
        for(i=0;i<num_threads;i++){
            vetIni2[i] = (i*chunk)+1;
            vetFim2[i] = ((i*chunk)+1)+chunk;
        }
    }else{
        chunk = (N-2)/num_threads;
        chunkPlus = chunk+1;
        sobra = (N-2)%num_threads;
        for(i=0;i<num_threads;i++){
            if(i < sobra){
                vetIni2[i] = (i*chunkPlus)+1;
                vetFim2[i] = ((i*chunkPlus)+1)+chunkPlus;
            }else{
                vetIni2[i] = vetFim2[i-1];
                vetFim2[i] = vetIni2[i]+chunk;
            }
        }
    }
}

void *jacobi(void *threadId){
    long int idThread = (long int) threadId;
    int iter, i, j;
    for(i = vetIni[idThread]; i < vetFim[idThread]; i++){
        A[i]   = 0.f;
        A[((N-1)*N)+i] = 0.f;
        yO[i] = sinf(pi * i / (N-1));
        A[(i*N)] = yO[i];
        A[(i*N)+N-1] = yO[i]*expf(-pi);        
        Anew[i] = 0.f;
        Anew[((N-1)*N)+i] = 0.f;
        Anew[i*N] = yO[i];
        Anew[(i*N)+N-1] = yO[i]*expf(-pi);
    }

    for(iter=0;iter<iter_max;iter++){
        for(j = vetIni2[idThread]; j < vetFim2[idThread]; j++){
            for(i = 1; i < N-1; i++ ){
                Anew[(j*N)+i] = 0.25f * ( A[(j*N)+i+1] + A[(j*N)+i-1]+ A[((j-1)*N)+i] + A[((j+1)*N)+i]);
            }
        }                
        pthread_barrier_wait(&cond);
		if(idThread == 0){
          	aux = A;
            A = Anew;
            Anew = aux;
        }
        pthread_barrier_wait(&cond);       		
	}
	return NULL;
}

int main(int argc, char** argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    long int i;
    N = atoi(argv[2]);
    A = malloc(sizeof(float)*N*N);
    Anew = malloc(sizeof(float)*N*N);
    yO = malloc(sizeof(float)*N);
    pi  = 2.0f * asinf(1.0f);

    memset(A, 0, N * N * sizeof(float));

    num_threads = atoi(argv[1]);
    
    pthread_t threads[num_threads];
    pthread_barrier_init(&cond, NULL, num_threads);

    vetFim =  malloc(sizeof(int)*num_threads);
    vetIni = malloc(sizeof(int)*num_threads);
    vetFim2 = malloc(sizeof(int)*num_threads);
    vetIni2 = malloc(sizeof(int)*num_threads);

    ajustaChunk1();
    ajustaChunk2();

    for(i=0;i<num_threads;i++){
        pthread_create(&threads[i], NULL, jacobi, (void *)i);
    }
    for(i=0;i<num_threads;i++){
        pthread_join(threads[i], NULL);
    }

    // set boundary conditions
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

    return 0;
}

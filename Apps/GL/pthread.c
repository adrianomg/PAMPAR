#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

unsigned short int *socIn, *socOut;
int NTotal, N, indice,numGeracoes = 100;
long int numThreads;
pthread_mutex_t mutexAjuste;
pthread_barrier_t cond;

void povoasocIn(FILE *input){
        int i, j, tam = NTotal-2;

        for(i=1; i<=tam; i++){
                for(j=1;j<=tam; j++){
                        fscanf(input, "%hu", &socIn[(i*NTotal)+j]);
                        socOut[(i*NTotal)+j] = 0;
                }
        }
}

void defineVizinho(){
        int i, j, NAux = NTotal - 2;
        socIn[0] = socIn[(NAux*NTotal)+NAux];
        socIn[NTotal-1] = socIn[(NAux*NTotal)+1];
        socIn[(NTotal*NTotal)-1] = socIn[(NTotal+1)];
        socIn[NTotal*(NTotal-1)] = socIn[NTotal+NAux];
        for(i = 1; i < NTotal -1; i++)
        {
            socIn[i] = socIn[i+(NAux*NTotal)];
            socIn[((NTotal*NTotal)+i)-NTotal] = socIn[NTotal+i];
        }
        for(j = 1; j < NTotal-1; j++)
        {
            socIn[NTotal*j] = socIn[(NTotal*j)+NAux];
            socIn[(NTotal*j)+NAux+1] = socIn[(NTotal*j)+1];
        }
}

void joga(int LineIni, int LineFim, long int idThread){
        int i, j, celulas, evolucao;
        int NAux = NTotal-2;
		unsigned short int *aux;
        for(evolucao = 0; evolucao < numGeracoes; evolucao++){
                for(i=LineIni; i<LineFim; i++){
                        for(j=1;j<NAux;j++){
                                celulas = socIn[((i*NAux)+j) - (NAux+1)] + socIn[((i*NAux)+j) - NAux] + socIn[((i*NAux)+j) - (NAux-1)] + socIn[(((i*NAux)+j) - 1)] + socIn[(((i*NAux)+j)+1)] + socIn[(((i*NAux)+j)+(NAux-1))] + socIn[(((i*NAux)+j)+(NAux))] + socIn[(((i*NAux)+j)+(NAux+1))];
                                switch(celulas){
                                        case 3:
                                                socOut[(i*NAux)+j] = 1;
                                                break;
                                        case 2:
                                                socOut[(i*NAux)+j] = socIn[(i*NAux)+j];
                                                break;
                                        default:
                                                socOut[(i*NAux)+j] = 0;
                                                break;
                                }

                        }
                }
                //isto aqui deve ser realizada por somente uma thread, as demais devem aguardar até o final!
				pthread_barrier_wait(&cond);
                if(idThread == 1){
						aux = socIn;
                        socIn = socOut;
						socOut = aux;
                        defineVizinho();
                }
				pthread_barrier_wait(&cond);
        }

}


void *gameoflife(void *threadId){
		long int thread;
		thread = (long int) threadId;
		if (thread == 1){
			joga(((NTotal/numThreads)*(thread-1))+1, ((NTotal/numThreads)*(thread)), thread);
		}else if(thread == numThreads){	
			joga(((NTotal/numThreads)*(thread-1)), NTotal, thread);
		}else{
			joga(((NTotal/numThreads)*(thread-1)), ((NTotal/numThreads)*(thread)), thread);
		}			
}

void imprime(){
        int vivas=0, i, j;
        for(i=1;i<NTotal-1;i++){
                for(j=1;j<NTotal-1;j++){
                        //printf("%hu\t", matriz[(i*N)+j]);
                        if(socIn[(i*NTotal)+j] == 1){
                                vivas++;
                        }
                }
                //printf("\n");
        }
        //printf("Número de celulas vivas: %d\n", vivas);
}

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-file> \n\n \tE.g. -> ./my_program 2048.txt 2\n\n");
		exit(1);
	}

    #ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
    #endif
    FILE *input;
    long int i;

    numThreads = atol(argv[1]);
    input = fopen(argv[2], "r");
    fscanf(input, "%d", &N);
    NTotal = N+2;
    socIn = (unsigned short int*) malloc(sizeof(unsigned short int)*NTotal*NTotal);
    socOut = (unsigned short int*) malloc(sizeof(unsigned short int)*NTotal*NTotal);
    povoasocIn(input);
    defineVizinho(); 
    
    pthread_t threads[numThreads];
    pthread_barrier_init(&cond, NULL, numThreads);

    for(i=1;i<=numThreads;i++){
            pthread_create(&threads[i], NULL, gameoflife, (void *)i);
    }
    for(i=1;i<=numThreads;i++){
            pthread_join(threads[i], NULL);
    }
    
    #ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
    #endif

    //imprime();
    fclose(input);
    free(socIn);
    free(socOut);
    return 0;
}

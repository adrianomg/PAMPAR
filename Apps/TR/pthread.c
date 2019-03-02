#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
struct ring{
    int X; //população de predadores
    int Y; //população de presas
};
typedef struct ring TipoRing;

TipoRing *jogo, *auxiliar;        
int N, presas, predadores, *vetIni, *vetFim, indice;
pthread_barrier_t cond;
pthread_mutex_t soma;

void inicializaMatriz(){
    int i, j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            jogo[(i*N)+j].X = predadores;
            jogo[(i*N)+j].Y = presas;
            auxiliar[(i*N)+j].X = 0;
            auxiliar[(i*N)+j].Y = 0;
        }
    }
}

void zeraAux(){
    int i, j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            auxiliar[(i*N)+j].X = 0;
            auxiliar[(i*N)+j].Y = 0;
        }
	}
}

void imprimeMatriz(){
    int i, j;
    printf("\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("%d | %d\t\t",jogo[(i*N)+j].X, jogo[(i*N)+j].Y);
        }
        printf("\n");
    }
}

void joga(int i, int j){
    int Xmenor=0, Xmaior=0, Ymenor=0, Ymaior=0, X, Y;
    float rx = 0.0798; //Taxa de morte Predadores por idade
    float ax = 0.0123; //Taxa de morte Predadores por superpopulação
    float bx = 0.0377; //Taxa de nascimento dos predadores
    float u = 0.12; //Taxa de migração dos predadores
    float ry = 0.0178; //Taxa de morte das presas
    float ay = 0.0998; //Taxa de presas mortas pelos predadores
    float by = 0.0100; //Taxa de nascimento das presas
    float v = 0.13; //Taxa de migração das presas

    float popX1, popY1, popX2, popY2;
    float fxy, gxy;

    X = jogo[(i*N)+j].X;
    Y = jogo[(i*N)+j].Y;

    fxy = X*(rx + (ax * X) + (bx * Y));
    gxy = Y*(ry + (ay * X) + (by * Y));

	// verificando  posicao anterior
	if ((i-1) < 0) {
		Xmenor =  jogo[((N-1)*N)+j].X;
	} else {
		Xmenor = jogo[((i-1)*N)+j].X;
	}

	if ((i+1) > (N-1)) {
		Xmaior = jogo[j].X;
	} else {
		Xmaior = jogo[((i+1)*N)+j].X;
	}

	popX1 = fxy +  u*(Xmenor - (2* X) + Xmaior);
	popY1 = gxy +  v*(Ymenor - (2* Y) + Ymaior);

	// verifica posicao final
	if ((j-1) < 0) {
		Ymenor =  jogo[(i*N)+N-1].Y;
	} else {
		Ymenor = jogo[(i*N)+j-1].Y;
	}

	if ((j+1) > (N-1)) {
		Ymaior = jogo[i*N].Y;
	} else {
		Ymaior = jogo[(i*N)+j+1].Y;
	}

	popX2 = fxy +  u*(Xmenor - (2* X) + Xmaior);
	popY2 = gxy +  v*(Ymenor - (2* Y) + Ymaior);

	float Xfinal = (popX1 + popX2)/2;
	float Yfinal = (popY1 + popY2)/2;

    auxiliar[(i*N)+j].X = Xfinal;
    auxiliar[(i*N)+j].Y = Yfinal;
}

void *jogaPthreads(void *threadId){
       
    int tam = ((N*2)-1); 
    int x=0, i, linha, coluna;
    long int idThread = (long int) threadId;

	for(x=0;x<tam;x++){
        pthread_barrier_wait(&cond);
        for(linha = vetIni[idThread]; linha < vetFim[idThread]; linha++){
            for(coluna = 0; coluna < N; coluna++){
                if(linha +  coluna == x){
                        joga(linha, coluna);
                }
            }
		}
		pthread_barrier_wait(&cond);
		for(linha = vetIni[idThread]; linha < vetFim[idThread]; linha++){
            for(coluna = 0; coluna < N; coluna++){
                if(linha +  coluna == x){
                    jogo[(linha*N)+coluna].X = auxiliar[(linha*N)+coluna].X;
                    jogo[(linha*N)+coluna].Y = auxiliar[(linha*N)+coluna].Y;
                }
            }
        }
    }
	return NULL;
}

void ajustaChunk(int numThreads){
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
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    N = atoi(argv[2]);
    presas = 100;
    predadores = 100;
    int numThreads = atoi(argv[1]);
	long int i;
    pthread_t threads[numThreads];
    jogo = malloc(sizeof(TipoRing)*N*N);
    auxiliar = malloc(sizeof(TipoRing)*N*N);        
    vetIni = malloc(sizeof(int)*numThreads);
    vetFim = malloc(sizeof(int)*numThreads);

    pthread_barrier_init(&cond, NULL, numThreads);

    inicializaMatriz();

    ajustaChunk(numThreads);

    zeraAux();

    for(i=0;i<numThreads;i++){
        pthread_create(&threads[i], NULL, jogaPthreads, (void *)i);
    }
    for(i=0;i<numThreads;i++){
        pthread_join(threads[i], NULL);
    }
   
    //imprimeMatriz();
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

    return 0;
}



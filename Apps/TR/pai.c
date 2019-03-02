#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <sys/time.h>
struct ring{
    int X; //população de predadores
    int Y; //população de presas
};
typedef struct ring TipoRing;

MPI_Comm intercommFilho;

void inicializaMatriz(TipoRing *jogo, int N, int predadores, int presas, TipoRing *nova){
    int i, j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            jogo[(i*N)+j].X = predadores;
            jogo[(i*N)+j].Y = presas;
            nova[(i*N)+j].X = 0;
            nova[(i*N)+j].Y = 0;
        }
    }
}

void zeraAux(TipoRing *nova, int N){
    int i, j;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            nova[(i*N)+j].X = 0;
            nova[(i*N)+j].Y = 0;
        }
    }
}

void imprimeMatriz(TipoRing *jogo, int N){
    int i, j;
    printf("\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("%d|%d  ",jogo[(i*N)+j].X, jogo[(i*N)+j].Y);
        }
        printf("\n");
    }
}

void joga(TipoRing *jogo, int i, int j, int N, TipoRing *nova){
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

    nova[(i*N)+j].X = Xfinal;
    nova[(i*N)+j].Y = Yfinal;
}

void ajustaChunk(int *vetChunk, int *vetFim, int *vetIni, int size, int N){
    int i, chunk, chunkPlus, sobra;
    //não existem sobras
    if(N%size == 0){
        chunk = N/size;
        for(i=0;i<size;i++){
                vetChunk[i] = chunk;
                vetIni[i] = i*chunk;
                vetFim[i] = vetIni[i] + chunk;
        }
    }else{
        chunk = N/size;
        chunkPlus = chunk+1;
        sobra = N%size;
        for(i=0;i<size;i++){
            if(i < sobra){
                vetIni[i] = i*chunkPlus;
                vetFim[i] = (i*chunkPlus)+chunkPlus;
                vetChunk[i] = chunkPlus;
            }else{
                vetIni[i] = vetFim[i-1];
                vetFim[i] = vetIni[i]+chunk;
                vetChunk[i] = chunk;
            }
        }
    }
}

int main(int argc, char **argv){
	
	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 2048 "$PWD/child"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    MPI_Init(&argc, &argv);
    MPI_Info localInfo;
    MPI_Info_create(&localInfo);
    MPI_Request req;
    MPI_Request reqs[128];

    //Inicialização para jogo
    int x = 0, i, linha, coluna, indice, N;

    N = atoi(argv[2]);

	char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);

    int presas = 100;
    int predadores = 100;
    int numProc = atoi(argv[1]);
    int tam = ((N*2) - 1);
    int size = numProc+1;
    int err[1] = {0};

    //Inicializacao para paralelização
	int *vetChunk = NULL, *vetIni = NULL, *vetFim = NULL;
    TipoRing *jogo, *auxiliar;
    vetChunk = malloc(sizeof(int)*size);
    vetFim = malloc(sizeof(int)*size);
    vetIni = malloc(sizeof(int)*size);

    ajustaChunk(vetChunk, vetFim, vetIni, size, N);

    //Alocação das estruturas e inicialização
    jogo = malloc(sizeof(TipoRing)*N*N);
    auxiliar = malloc(sizeof(TipoRing)*N*N);        
    inicializaMatriz(jogo, N, predadores, presas, auxiliar);
    zeraAux(auxiliar, N);

    MPI_Comm_spawn(bin, argv, numProc, localInfo, 0, MPI_COMM_SELF, &intercommFilho, err);



	for(i=0;i<numProc;i++){
		MPI_Send(&jogo[0], N*N, MPI_INT, i, 49, intercommFilho);
	}
        
	//Enviar dados para todos os processos, cada parte para cada um!
    for(x=0; x<tam; x++){
        for(linha=vetIni[0];linha<vetFim[0];linha++){
            for(coluna=0;coluna<N;coluna++){
                if(linha + coluna == x){
                    joga(jogo, linha,coluna, N, auxiliar);
                }
            }
        }
        MPI_Irecv(&jogo[(vetIni[1]*N)], N, MPI_INT, 0, 9, intercommFilho, &req);
        for(linha=vetIni[0];linha<vetFim[0];linha++){
            for(coluna=0;coluna<N;coluna++){
                if(linha + coluna == x){
                    jogo[(linha*N)+coluna].X = auxiliar[(linha*N)+coluna].X;
                    jogo[(linha*N)+coluna].Y = auxiliar[(linha*N)+coluna].Y;
                }
            }
        }

        MPI_Send(&jogo[(vetFim[0]-1)*N], N, MPI_INT, 0, 49, intercommFilho);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    for(i=0;i<numProc;i++){
        MPI_Irecv(&jogo[(vetIni[1]*N)], vetChunk[1]*N, MPI_INT, i, 9, intercommFilho, &reqs[i]);
    }
    int source=0;
    for(i=0;i<numProc;i++){
        MPI_Waitany(numProc, reqs, &source, MPI_STATUS_IGNORE);
	}
    MPI_Finalize();
    #ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

    return 0;
}



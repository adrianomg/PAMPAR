#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct ring{
    int X; //população de predadores
    int Y; //população de presas
};
typedef struct ring TipoRing;

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
            printf("%d | %d\t\t",jogo[(i*N)+j].X, jogo[(i*N)+j].Y);
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

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    int x = 0;
    int i, linha, coluna;
    int N = atoi(argv[1]);
    int presas = 100;
    int predadores = 100;

    TipoRing *jogo = malloc(sizeof(TipoRing)*N*N);
    TipoRing *auxiliar = malloc(sizeof(TipoRing)*N*N);        

    inicializaMatriz(jogo,N,predadores,presas, auxiliar);

    zeraAux(auxiliar, N);
    
   for(x=0;x< ((N*2)-1); x++){
        for(linha=0;linha<N;linha++){
		    for(coluna=0;coluna<N;coluna++){
	            if(linha + coluna == x){
                    joga(jogo, linha,coluna, N, auxiliar);
	            }
		    }
  		}
 		for(linha=0;linha<N;linha++){
            for(coluna=0;coluna<N;coluna++){
                if(linha + coluna == x){
                    jogo[(linha*N)+coluna].X = auxiliar[(linha*N)+coluna].X;
                    jogo[(linha*N)+coluna].Y = auxiliar[(linha*N)+coluna].Y;
                }
            }
        }
    }      
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

    return 0;
}



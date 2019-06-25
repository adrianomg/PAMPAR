#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* Função para popular a matriz, através de um .txt de entrada */
void povoaMatriz(unsigned short int *matrizOut, unsigned short int *matriz, int NTotal, FILE *dadosEntrada)
{
        int i, j, tam = NTotal - 2;
        for(i=1; i<=tam; i++)
        {
                for(j=1; j<=tam; j++)
                {
                        fscanf(dadosEntrada,"%hu",&matriz[(i*NTotal)+j]);
                        matrizOut[(i*NTotal)+j] = 0;
                }
        }
}

/* Função para definir os vizinhos */
void defineVizinho(unsigned short int *matriz, int NTotal){
        int i, j, N = NTotal - 2;
        matriz[0] = matriz[(N*NTotal)+N];
        matriz[NTotal-1] = matriz[(N*NTotal)+1];
        matriz[(NTotal*NTotal)-1] = matriz[(NTotal+1)];
        matriz[NTotal*(NTotal-1)] = matriz[NTotal+N];
        for(i = 1; i < NTotal -1; i++)
        {
            matriz[i] = matriz[i+(N*NTotal)];
            matriz[((NTotal*NTotal)+i)-NTotal] = matriz[NTotal+i];
        }
        for(j = 1; j < NTotal-1; j++)
        {
            matriz[NTotal*j] = matriz[(NTotal*j)+N];
            matriz[(NTotal*j)+N+1] = matriz[(NTotal*j)+1];
        }
}

/*void imprime(unsigned short int *matriz, int N){
        int vivas=0, i, j;
        for(i=1;i<N-1;i++){
                for(j=1;j<N-1;j++){
                        if(matriz[(i*N)+j] == 1){
                                vivas++;
                        }
                }
        }
        printf("Número de celulas vivas: %d\n", vivas);
}

void imprimeBordas(unsigned short int *matriz, int N){
        int vivas=0, i, j;
        for(i=0;i<N;i++){
                for(j=0;j<N;j++){
                        printf("%hu\t", matriz[(i*N)+j]);
                        if(matriz[(i*N)+j] == 1){
                                vivas++;
                        }
                }
                printf("\n");
        }
        printf("Número de celulas vivas: %d\n", vivas);
}*/

void joga(unsigned short int *socIn, unsigned short int *socOut, int Ntotal, int numGeracoes){
    int i, j, celulas, iter;
    int N = Ntotal;
	unsigned short int *aux;
        for(iter=0; iter<numGeracoes; iter++){
                for(i=1;i<N;i++){
                        for(j=1;j<N;j++){
                                celulas = socIn[((i*N)+j) - (N+1)] + socIn[((i*N)+j) - N] + socIn[((i*N)+j) - (N-1)] + socIn[(((i*N)+j) - 1)] + socIn[(((i*N)+j)+1)] + socIn[(((i*N)+j)+(N-1))] + socIn[(((i*N)+j)+(N))] + socIn[(((i*N)+j)+(N+1))];
                                switch(celulas){
                                        case 3:
                                                socOut[(i*N)+j] = 1;
                                                break;
                                        case 2:
                                                socOut[(i*N)+j] = socIn[(i*N)+j];
                                                break;
                                        default:
                                                socOut[(i*N)+j] = 0;
                                                break;
                                }
                        }
                }
		aux = socIn;
                socIn = socOut;
		socOut = aux;
                defineVizinho(socIn, N);
               }
       
}


int main(int argc, char **argv){
	
	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-file>\n\n \tE.g. -> ./my_program 2048.txt\n\n");
		exit(1);
	}

    #ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
        #endif
    long long valor;

    unsigned short int *socIn, *socOut;
    FILE *input;
    int numGeracoes = 100, N, Ntotal;
    input = fopen(argv[1], "r");
    fscanf(input, "%d", &N);
    Ntotal = N+2;
    socIn = (unsigned short int*) malloc(sizeof(unsigned short int)*Ntotal*Ntotal);
    socOut = (unsigned short int*) malloc(sizeof(unsigned short int)*Ntotal*Ntotal);

    povoaMatriz(socOut, socIn, Ntotal, input);
    defineVizinho(socIn, Ntotal);
    //imprimeBordas(socIn, Ntotal);
    
    joga(socIn, socOut, Ntotal, numGeracoes);

    #ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
    #endif

   //imprime(socIn, N);

    fclose(input);
    free(socIn);
    free(socOut);
    return 0;
}

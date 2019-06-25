#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int envia = 0, recebe = 0;

void povoaMatriz(unsigned short int *matriz, int NTotal, FILE *dadosEntrada){
    int i, j, tam = NTotal - 2;
    for(i=1; i<=tam; i++){
        for(j=1; j<=tam; j++){
            fscanf(dadosEntrada,"%hu",&matriz[(i*NTotal)+j]);
        }
    }
}

void defineVizinho(unsigned short int *matriz, int NTotal){
    int i, j, N = NTotal - 2;
    matriz[0] = matriz[(N*NTotal)+N];
    matriz[NTotal-1] = matriz[(N*NTotal)+1];
    matriz[(NTotal*NTotal)-1] = matriz[(NTotal+1)];
    matriz[NTotal*(NTotal-1)] = matriz[NTotal+N];
    for(i = 1; i < NTotal -1; i++){
        matriz[i] = matriz[i+(N*NTotal)];
        matriz[((NTotal*NTotal)+i)-NTotal] = matriz[NTotal+i];
    }
    for(j = 1; j < NTotal-1; j++){
        matriz[NTotal*j] = matriz[(NTotal*j)+N];
        matriz[(NTotal*j)+N+1] = matriz[(NTotal*j)+1];
    }
}

void calculaCelulas(unsigned short int *matriz, int nTotal){
	int i, j;
	long int celulasVivas = 0;
	for(i = 1; i< nTotal - 1; i++){
        	for(j = 1; j < nTotal - 1; j++){
        		celulasVivas = celulasVivas + matriz[(i*nTotal)+j];	
        	}//printf("\n");
	}
	printf("celulas vivas: %ld\n", celulasVivas);
}

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-file> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 2048.txt 3 \"$PWD/child\"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
    #endif
	int pid, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	MPI_Comm interCommFilho;

	FILE *matrizEntrada = NULL;
	
	char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);
	  
	int nFilhos=0, linhasComputaveis=0, linhasTotais=0, sobra=0, i=0;
	 
	unsigned short int *vetChunk=NULL, *gameIn=NULL, *gameOut=NULL;
	  
	int *inicio=NULL;
	  
	matrizEntrada = fopen(argv[2],"r");
 
	if(matrizEntrada == NULL){
		printf("Não foi possível carregar o arquivo!\n");
		MPI_Finalize();
		return 1;	
	}

	fscanf(matrizEntrada, "%d", &linhasComputaveis);

	linhasTotais = linhasComputaveis+2;

	nFilhos = atoi(argv[1]);
	int err[1];
	//geracoes = 100;

	vetChunk = (unsigned short int*) malloc(sizeof(unsigned short int)*(nFilhos));
	inicio = (int*) malloc(sizeof(int)*(nFilhos));
		
	if(linhasComputaveis%nFilhos == 0){
		for(i=0;i<nFilhos;i++){
		      vetChunk[i] = linhasComputaveis/nFilhos;
		}
	}else{
		sobra = linhasComputaveis%nFilhos;
		for(i=0;i<nFilhos;i++){
			if(i<sobra)
				vetChunk[i] = (linhasComputaveis/nFilhos)+1;
		    else
			    vetChunk[i] = linhasComputaveis/nFilhos;	      
	   }
	}

	for(i=0;i<nFilhos;i++){
		   inicio[i] = 0;
	}
	for(i=1;i<nFilhos;i++){
		  inicio[i] = (inicio[i-1] + ((vetChunk[i-1]+2)*linhasTotais) - (linhasTotais*2));
	}

	gameIn = (unsigned short int*) malloc(sizeof(unsigned short int)*(linhasTotais*linhasTotais));
	gameOut = (unsigned short int*) malloc(sizeof(unsigned short int)*(linhasTotais*linhasTotais));
	povoaMatriz(gameIn, linhasTotais, matrizEntrada);
	defineVizinho(gameIn, linhasTotais);

	MPI_Comm_spawn(bin, argv, nFilhos, localInfo, pid, MPI_COMM_SELF, &interCommFilho, err);

	for(i=0;i<nFilhos;i++){
		MPI_Send(&linhasComputaveis, 1, MPI_INT, i, i+99, interCommFilho);
	}

	for(i=0;i<nFilhos;i++){
		MPI_Send(&gameIn[inicio[i]], ((vetChunk[i]+2)*linhasTotais), MPI_UNSIGNED_SHORT, i, i+99, interCommFilho);
	}
	for(i=0;i<nFilhos;i++){
		MPI_Recv(&gameOut[inicio[i]+linhasTotais], vetChunk[i]*linhasTotais, MPI_UNSIGNED_SHORT, i, 99, interCommFilho, MPI_STATUS_IGNORE);
	}

	calculaCelulas(gameOut, linhasTotais);

	MPI_Finalize();
	
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
    #endif
    
	free(vetChunk);
	free(inicio);
	free(gameIn);
	free(gameOut);
	return 0;
}

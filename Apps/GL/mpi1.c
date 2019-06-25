#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

MPI_Status status;

void povoaMatriz(unsigned short int *matriz, int NTotal, FILE *dadosEntrada){
    int i, j, tam = NTotal - 2;
    for(i=1; i<=tam; i++)
    {
        for(j=1; j<=tam; j++)
        {
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

void computaTrabalhador(int maxLinhas, unsigned short int *gameIn, unsigned short int *gameOut, int linhasTotais){
    int i, j, celulas=0;
  
    for(i=1;i<=maxLinhas;i++){
	  for(j=1;j<linhasTotais;j++){
		celulas = gameIn[((i*linhasTotais)+j) - (linhasTotais+1)] + gameIn[((i*linhasTotais)+j) - linhasTotais] + gameIn[((i*linhasTotais)+j) - (linhasTotais-1)] + gameIn[(((i*linhasTotais)+j) - 1)] + gameIn[(((i*linhasTotais)+j)+1)] + gameIn[(((i*linhasTotais)+j)+(linhasTotais-1))] + gameIn[(((i*linhasTotais)+j)+(linhasTotais))] + gameIn[(((i*linhasTotais)+j)+(linhasTotais+1))];
		switch(celulas)
		{
		      case 3:
			  gameOut[(i*linhasTotais)+j] = 1;
			  break;
		      case 2:
			  gameOut[(i*linhasTotais)+j] = gameIn[(i*linhasTotais)+j];
			  break;
		      default:
			  gameOut[(i*linhasTotais)+j] = 0;
		}
	  }
    } 
}


void trabalhaMestre(unsigned short int *gameIn, unsigned short int *gameOut, unsigned short int *vetChunk, int numTrabalhadores, int linhasComputaveis, int *inicio){
     int procDest, tamDados, posicao, idProcEnvia, linhasTotais = linhasComputaveis+2, start=0;
     
     for(procDest=1;procDest<=numTrabalhadores;procDest++){
	  tamDados = (vetChunk[procDest]+2)*linhasTotais;
	  MPI_Send(&gameIn[inicio[procDest]], tamDados, MPI_UNSIGNED_SHORT, procDest, 99, MPI_COMM_WORLD);
     }
     for(procDest=1;procDest<=numTrabalhadores;procDest++){
	  MPI_Recv(&posicao, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
	  idProcEnvia = status.MPI_SOURCE;
	  MPI_Recv(&gameOut[inicio[idProcEnvia]+linhasTotais],vetChunk[idProcEnvia]*linhasTotais ,MPI_UNSIGNED_SHORT, idProcEnvia, 99, MPI_COMM_WORLD, &status);
     }
}

void trabalhaEscravos(unsigned short int *gameIn, unsigned short int *gameOut, int termina, unsigned short int *vetChunk, int numTrabalhadores, int linhasComputaveis, int *inicio, int pid, int evolucoes, unsigned short int *bordaAnt, unsigned short int *bordaPos){
   
    int celulas = 0, tamDados, i, j, linhasTotais = linhasComputaveis+2, cont=0, index;
    unsigned short int *aux, *aux2, *aux3;
    MPI_Request req[2];
    if(evolucoes == 1){
	  tamDados = (vetChunk[pid]+2)*linhasTotais;
	  MPI_Recv(&gameIn[0], tamDados, MPI_UNSIGNED_SHORT, 0, 99, MPI_COMM_WORLD, &status);
    }
    
    computaTrabalhador(vetChunk[pid], gameIn, gameOut, linhasTotais);
        
    if(termina == 1){
	  MPI_Send(&cont, 1, MPI_INT, 0, 99, MPI_COMM_WORLD);
	  MPI_Send(&gameOut[linhasTotais], (vetChunk[pid]*linhasTotais), MPI_UNSIGNED_SHORT, 0, 99, MPI_COMM_WORLD);
    }
    else{
	  for(j=1;j<=vetChunk[pid]; j++){
		gameOut[linhasTotais*j] = gameOut[(linhasTotais*j)+linhasComputaveis];
		gameOut[(linhasTotais*j)+linhasComputaveis+1] = gameOut[(linhasTotais*j)+1];
	  }

	  	  
	  MPI_Irecv(&gameOut[0], linhasTotais, MPI_UNSIGNED_SHORT, bordaAnt[pid], 99, MPI_COMM_WORLD, &req[0]);
	  MPI_Irecv(&gameOut[(vetChunk[pid]+1)*linhasTotais], linhasTotais, MPI_UNSIGNED_SHORT, bordaPos[pid] , 99, MPI_COMM_WORLD, &req[1]); 
	  
	  MPI_Send(&gameOut[linhasTotais], linhasTotais, MPI_UNSIGNED_SHORT, bordaAnt[pid], 99, MPI_COMM_WORLD);
	  MPI_Send(&gameOut[((vetChunk[pid]+2)*linhasTotais) - (linhasTotais*2)],linhasTotais, MPI_UNSIGNED_SHORT, bordaPos[pid], 99, MPI_COMM_WORLD);
	    
	  for(i=0;i<2;i++){
		MPI_Waitany(2, req, &index, MPI_STATUS_IGNORE);
	  }
    }
}

void calculaCelulas(unsigned short int *matriz, int nTotal){
    int i, j, celulasVivas = 0;
    for(i = 1; i< nTotal - 1; i++)
    {
        for(j = 1; j < nTotal - 1; j++)
        {
            celulasVivas = celulasVivas + matriz[(i*nTotal)+j];
        }
    }
    //printf("Numero total de células Vivas = %d\n",celulasVivas);
}

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-file>\n\n \tE.g. -> mpirun -np 2 ./my_program 2048.txt\n\n");
		exit(1);
	}

    #ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
    #endif
    
	int totalProc, pid;
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    FILE *matrizEntrada = NULL;

    int sobra, geracoes, i, linhasComputaveis, linhasTotais, numTrabalhadores, totalLinhaBloco = 0, termina, evolucoes=0;
    
    unsigned short int *vetChunk, *mapDados, *gameIn, *gameOut, *bordaAnt, *bordaPos, *aux;
    int *inicio;
    
    matrizEntrada = fopen(argv[1],"r");
    geracoes = 100;
    
    fscanf(matrizEntrada, "%d", &linhasComputaveis);
    linhasTotais = linhasComputaveis + 2;
    
    numTrabalhadores = totalProc - 1;
    
    vetChunk = (unsigned short int*) malloc(sizeof(unsigned short int)*(totalProc));
    inicio = (int*) malloc(sizeof(int)*(totalProc));
  
    if(linhasComputaveis%numTrabalhadores == 0){
	  for(i=1;i<=numTrabalhadores;i++){
		  vetChunk[i] = linhasComputaveis/numTrabalhadores;
	  }
    }else{
	  sobra = linhasComputaveis%numTrabalhadores;
	  for(i=1;i<=numTrabalhadores;i++){
		if(i<=sobra){
		      vetChunk[i] = (linhasComputaveis/numTrabalhadores)+1;
		}else{
		      vetChunk[i] = linhasComputaveis/numTrabalhadores;
		}	    
	  }      
    }
    
    for(i=1;i<=numTrabalhadores;i++){
	  inicio[i] = 0;
    }
    for(i=2;i<=numTrabalhadores;i++){
	  inicio[i] = (inicio[i-1] + ((vetChunk[i-1]+2)*linhasTotais) - (linhasTotais*2)) ;
    }
    
    if(pid == 0){
	  gameOut = (unsigned short int*) malloc(sizeof(unsigned short int)*(linhasTotais*linhasTotais));
	  gameIn = (unsigned short int*) malloc(sizeof(unsigned short int)*(linhasTotais*linhasTotais));
	  povoaMatriz(gameIn, linhasTotais, matrizEntrada);
	  defineVizinho(gameIn, linhasTotais);
    }else{
	  gameOut = (unsigned short int*) malloc(sizeof(unsigned short int)*( (vetChunk[pid]+2) * linhasTotais));
	  gameIn = (unsigned short int*) malloc(sizeof(unsigned short int)*( (vetChunk[pid]+2) * linhasTotais));
	  bordaAnt = (unsigned short int*) malloc(sizeof(unsigned short int)*(totalProc));
	  bordaPos = (unsigned short int*) malloc(sizeof(unsigned short int)*(totalProc));
	  bordaAnt[1] = numTrabalhadores;
	  for(i=2;i<=numTrabalhadores;i++){
		bordaAnt[i] = i-1;
	  }
	  bordaPos[numTrabalhadores] = 1;
	  for(i=1;i<numTrabalhadores;i++){
		bordaPos[i] = i+1;
	  }
    }
    
    if(pid == 0){
	  trabalhaMestre(gameIn, gameOut, vetChunk, numTrabalhadores, linhasComputaveis, inicio);
	  calculaCelulas(gameOut, linhasTotais);
      
    }else{
	 for(evolucoes=1;evolucoes<=geracoes;evolucoes++){
	      if(evolucoes == geracoes){
		    termina = 1;		  
	      }
	      trabalhaEscravos(gameIn, gameOut, termina, vetChunk, numTrabalhadores, linhasComputaveis, inicio, pid, evolucoes, bordaAnt, bordaPos);
	      	  aux = gameOut;
	  gameOut = gameIn;
	  gameIn = aux;
   	 }
    }

    MPI_Finalize();    
    if(pid == 0){
        #ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
        #endif
    }
            
    return 0;
}

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

MPI_Comm comm_pai=0;
MPI_Status status;

int envia = 0, recebe = 0;

void computaTrabalho(int maxLinhas, unsigned short int *gameOut, unsigned short int *gameIn, int linhasTotais){
      int i, j, celulas;
	//printf("MAS NÃO É POSSÍVEL!!!\n");
      for(i=1;i<=maxLinhas;i++){
	    for(j=1;j<linhasTotais;j++){
		//printf("Temos um problema Huston\n");
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

void trabalhaEscravos(unsigned short int *gameIn, unsigned short int *gameOut, int linhasComputaveis, int termina, unsigned short int *bordaAnt, unsigned short int *bordaPos, unsigned short int *vetChunk, int pid, int geracoes){
	
      MPI_Request req[2];
      int linhasTotais = linhasComputaveis+2, i, index[2];

      if(geracoes == 1){
	    printf("Filho %d tenta receber em %lu de tamanho %d\n", pid, sizeof(gameIn[0]), ((vetChunk[pid]+2)*linhasTotais));
	    MPI_Recv(&gameIn[0], ((vetChunk[pid]+2)*linhasTotais), MPI_UNSIGNED_SHORT, 0, pid+99, comm_pai, &status);
	    printf("Mas  o filho %d logo viu que não era nada demais\n", pid);
      }

      computaTrabalho(vetChunk[pid], gameOut, gameIn, linhasTotais);
      printf("Os filho %d orgulhosamente computou seu trabalho\n", pid);
     
      if(termina == 1){
		//printf("%d %d\n", gameOut[linhasTotais], vetChunk[pid]*linhasTotais);
		MPI_Send(&gameOut[linhasTotais], vetChunk[pid]*linhasTotais, MPI_UNSIGNED_SHORT, 0, 99, comm_pai);
		printf("Processo %d envia %d de tamanho %d\n", pid, gameOut[linhasTotais], vetChunk[pid]*linhasTotais);
      }
      else{
	    for(i=1;i<=vetChunk[pid];i++){
		  gameOut[linhasTotais*i] = gameOut[(linhasTotais*i)+linhasComputaveis];
		  gameOut[(linhasTotais*i)+linhasComputaveis+1] = gameOut[(linhasTotais*i)+1];
	    }
	    
	    MPI_Irecv(&gameOut[0], linhasTotais, MPI_UNSIGNED_SHORT, bordaAnt[pid], 99, MPI_COMM_WORLD, &req[0]);
	    MPI_Irecv(&gameOut[(vetChunk[pid]+1)*linhasTotais], linhasTotais, MPI_UNSIGNED_SHORT, bordaPos[pid], 99, MPI_COMM_WORLD, &req[1]);
	    
	    MPI_Send(&gameOut[linhasTotais], linhasTotais, MPI_UNSIGNED_SHORT, bordaAnt[pid], 99, MPI_COMM_WORLD);
	    MPI_Send(&gameOut[((vetChunk[pid]+2)*linhasTotais) - (linhasTotais*2)],linhasTotais, MPI_UNSIGNED_SHORT, bordaPos[pid], 99, MPI_COMM_WORLD);
	    
	    for(i=0;i<2;i++){
		  MPI_Waitany(2, req, &index[i], MPI_STATUS_IGNORE);
	    }
      }
}

int main(int argc, char **argv){

    //argv+=1;
    int pid, nFilhos;
    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&comm_pai);
    MPI_Comm_size(MPI_COMM_WORLD, &nFilhos);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	printf("FILHO %d CRIADO\n", pid);
    int sobra=0, linhasComputaveis=0, linhasTotais=0, geracoes=0, termina = 0, i=0;
    
    unsigned short int *gameIn=NULL, *gameOut=NULL, *vetChunk=NULL, *bordaAnt=NULL, *bordaPos=NULL, *aux=NULL;
    
    geracoes = 100;
    MPI_Recv(&linhasComputaveis, 1, MPI_INT, 0, pid+99, comm_pai, &status);
 
	printf("Do pai veio pro filho %d %d linhas computáveis\n", pid, linhasComputaveis);   
    linhasTotais = linhasComputaveis+2;
    
    vetChunk = (unsigned short int*) malloc(sizeof(unsigned short int)*nFilhos);
    if(linhasComputaveis % nFilhos== 0){
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
 
	printf("Filho alocou gameIn de tamanho %d\n", (vetChunk[pid]+2)*linhasTotais);
    gameIn = (unsigned short int*) malloc(sizeof(unsigned short int)*((vetChunk[pid]+2)*linhasTotais));
    gameIn = (unsigned short int*) malloc(sizeof(unsigned short int)*((vetChunk[pid]+2)*linhasTotais));
    gameOut = (unsigned short int*) malloc(sizeof(unsigned short int)*((vetChunk[pid]+2)*linhasTotais));
    bordaAnt = (unsigned short int*) malloc(sizeof(unsigned short int)*(nFilhos));
    bordaPos = (unsigned short int*) malloc(sizeof(unsigned short int)*(nFilhos));

    bordaAnt[0] = (unsigned short int) nFilhos-1;
    bordaPos[nFilhos-1] = 0;
    for(i=1;i<nFilhos;i++){
	  bordaAnt[i] = i-1;
    }
    for(i=0;i<nFilhos-1;i++){
	  bordaPos[i] = i+1;
    }

    for(i=1;i<=geracoes;i++){
	  if(i==geracoes)
		termina=1;
	  trabalhaEscravos(gameIn, gameOut, linhasComputaveis, termina, bordaAnt, bordaPos, vetChunk, pid, i);
	  aux = gameIn;
	  gameIn = gameOut;
	  gameOut = aux;
		
	//printf("Nao deu pau %d \n", i);
    }
	//MPI_Barrier(comm_pai);
    MPI_Finalize();
    return 0;
}

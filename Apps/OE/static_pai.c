#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 225000

int *vetIndices, *vetor, size, indice;
MPI_Comm interComm;
MPI_Request request[10];

void sortPar(int rank){
		int aux, i;
		for(i=vetIndices[(rank*size)+rank]; i<vetIndices[size+rank]; i = i+2){
				if(vetor[i] > vetor[i+1]){
						aux = vetor[i];
						vetor[i] = vetor[i+1];
						vetor[i+1] = aux;
				}
		}

}

void sortImpar(int rank){
		int aux, i;
		for(i=vetIndices[(size*2)+rank]; i<vetIndices[(size*3)+rank]; i = i+2){
				if(vetor[i] > vetor[i+1]){
						aux = vetor[i];
						vetor[i] = vetor[i+1];
						vetor[i+1] = aux;
				}
		}
}


void defineChunk(int size){
		if(size == 2){
				vetIndices[0] = 0; vetIndices[2] = 74999; vetIndices[4] = 1; vetIndices[6] = 74998;
				vetIndices[1] = 75000; vetIndices[3] = N-1; vetIndices[5] = 74999; vetIndices[7] = N-1;

		}else if(size == 3){
				vetIndices[0] = 0; vetIndices[3] = 49999; vetIndices[6] = 1; vetIndices[9] = 49998;
				vetIndices[1] = 50000; vetIndices[4] = 99999; vetIndices[7] = 49999; vetIndices[10] = 99999;
				vetIndices[2] = 100000; vetIndices[5] = N-1; vetIndices[8] = 99999; vetIndices[11] = N-1;							
		}else if(size == 4){
				vetIndices[0] = 0; vetIndices[4] = 37499; vetIndices[8] = 1; vetIndices[12] = 37498;
				vetIndices[1] = 37500; vetIndices[5] = 74999; vetIndices[9] = 37499; vetIndices[13] = 74998;
				vetIndices[2] = 75000; vetIndices[6] = 112499; vetIndices[10] = 74999; vetIndices[14] = 112498;
				vetIndices[3] = 112500; vetIndices[7] = N-1; vetIndices[11] = 112499; vetIndices[15] = N-1;
		}else if(size == 8){
				vetIndices[0] = 0; vetIndices[8] = 18749; vetIndices[16] = 1; vetIndices[24] = 18748;
				vetIndices[1] = 18750; vetIndices[9] = 37499; vetIndices[17] = 18749; vetIndices[25] = 37498;
				vetIndices[2] = 37500; vetIndices[10] = 56249; vetIndices[18] = 37499 ;vetIndices[26] = 56248;
				vetIndices[3] = 56250; vetIndices[11] = 74999; vetIndices[19] = 56249 ;vetIndices[27] = 74998;
				vetIndices[4] = 75000; vetIndices[12] = 93749; vetIndices[20] = 74999 ;vetIndices[28] = 93748;
				vetIndices[5] = 93750; vetIndices[13] = 112499; vetIndices[21] = 93749 ;vetIndices[29] = 112498;
				vetIndices[6] = 112500; vetIndices[14] = 131249; vetIndices[22] = 112499 ;vetIndices[30] = 131248;
				vetIndices[7] = 131250; vetIndices[15] = N-1; vetIndices[23] = 131249 ;vetIndices[31] = N-1;
		}
}


void bubble2(){
		int i, source;
		for(i=0;i<(N/2)+1;i++){
				sortPar(0);
				MPI_Send(&vetor[74999], 1, MPI_INT, 0, 99, interComm);
				MPI_Irecv(&vetor[74999], 1, MPI_INT, 0, 99, interComm, &request[0]);
				sortImpar(0);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}
		MPI_Recv(&vetor[74999], N- 74999+1, MPI_INT, 0, 99, interComm, MPI_STATUS_IGNORE);
		
}

void bubble3(){
		int i, source;
		for(i=0;i<(N/2)+2;i++){				
				sortPar(0);				
				MPI_Send(&vetor[49999], 1, MPI_INT, 0, 99, interComm);
				MPI_Irecv(&vetor[49999], 1, MPI_INT, 0, 99, interComm, &request[0]);
				sortImpar(0);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}

		MPI_Irecv(&vetor[49999], 50000, MPI_INT, 0, 99, interComm, &request[0]);
		MPI_Irecv(&vetor[99999], 50001, MPI_INT, 1, 100, interComm, &request[1]);
		for(i=0;i<2;i++){
				MPI_Waitany(2, request, &source, MPI_STATUS_IGNORE);
		}
}

void bubble4(){
		int i, source;
		for(i=0;i<(N/2)+1;i++){
				sortPar(0);
				MPI_Send(&vetor[37499], 1, MPI_INT, 0, 99, interComm);
				MPI_Irecv(&vetor[37499], 1, MPI_INT, 0, 99, interComm, &request[0]);
				sortImpar(0);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}
		MPI_Irecv(&vetor[37499], 37500, MPI_INT, 0, 99, interComm, &request[0]);
		MPI_Irecv(&vetor[74999], 37500, MPI_INT, 1, 199, interComm, &request[1]);
		MPI_Irecv(&vetor[112499], 37501, MPI_INT, 2, 299, interComm, &request[2]);

		for(i=0;i<3;i++){
				MPI_Waitany(3, request, &source, MPI_STATUS_IGNORE);
		}

}

void bubble8(){
		int i, source;
		for(i=0;i<(N/2)+1;i++){
				sortPar(0);
						MPI_Send(&vetor[18749], 1, MPI_INT, 0, 99, interComm);
						MPI_Irecv(&vetor[18749], 1, MPI_INT, 0, 99, interComm, &request[0]);
				sortImpar(0);
						MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}
		MPI_Irecv(&vetor[18749], 18750, MPI_INT, 0, 99, interComm, &request[0]);
		MPI_Irecv(&vetor[37499], 18750, MPI_INT, 1, 199, interComm, &request[1]);
		MPI_Irecv(&vetor[56249], 18750, MPI_INT, 2, 299, interComm, &request[2]);
		MPI_Irecv(&vetor[74999], 18750, MPI_INT, 3, 399, interComm, &request[3]);
		MPI_Irecv(&vetor[93749], 18750, MPI_INT, 4, 499, interComm, &request[4]);
		MPI_Irecv(&vetor[112499], 18750, MPI_INT, 5, 599, interComm, &request[5]);
		MPI_Irecv(&vetor[131249], 18751, MPI_INT, 6, 699, interComm, &request[6]);

		for(i=0;i<7;i++){
				MPI_Waitany(7, request, &source, MPI_STATUS_IGNORE);
		}

}

void leEntrada(){
		int i;
		for(i=0;i<N;i++){
				vetor[i] = N-i;
		}
}


int main(int argc, char **argv){
		#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
		int numFilhos = atoi(argv[1]);
		char bin[] = "Apps/OE/filho";
		size = numFilhos+1;

		MPI_Init(&argc, &argv);
      		MPI_Info localInfo;
      		MPI_Info_create(&localInfo);
		long long valor;

		vetor = malloc(sizeof(int)*N);
		vetIndices = malloc(sizeof(int)*(size)*4);
		defineChunk(numFilhos+1);
		
		leEntrada();
		
		int i, err[1];

		for(i=argc;i<7;i++){
				argv[i] = malloc(sizeof(char)*20);
		}

		sprintf(argv[4], "%d", numFilhos);
		sprintf(argv[5], "%d", indice);

		MPI_Comm_spawn(bin, argv, numFilhos, localInfo, 0, MPI_COMM_SELF, &interComm, err);

		numFilhos = atoi(argv[1]);
		for(i=0;i<numFilhos;i++){
				MPI_Send(&vetIndices[0],size*4, MPI_INT, i, 99, interComm);
		}
		if(numFilhos == 1)
				bubble2();
		else if(numFilhos == 2)
				bubble3();
		else if(numFilhos == 3)
				bubble4();
		else if(numFilhos == 7)
				bubble8();


		MPI_Finalize();
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif
}

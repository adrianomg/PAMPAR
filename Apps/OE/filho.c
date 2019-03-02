#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef PAPI
	#include <papi.h>
#endif

MPI_Comm interCommPai;
MPI_Request request[3];
long int N = 0, *vetor = NULL, *vetIndice = NULL, size = 0, indice = 0;

void leEntrada(){
	long int i;
	for(i=0;i<N;i++){
		vetor[i] = N-i;
	}
}

void sortPar(int rank){
	long int aux, i;
	for(i=vetIndice[rank]; i<vetIndice[(rank+size)]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}

}

void sortImpar(int rank){
	long int aux, i;
	for(i=vetIndice[(size*2)+rank]; i<vetIndice[(size*3)+rank]; i = i+2){
		if(vetor[i] > vetor[i+1]){
			aux = vetor[i];
			vetor[i] = vetor[i+1];
			vetor[i+1] = aux;
		}
	}
}

void bubbleSort(int rank){
	long int i;
	long int aux = N/size;
	int id = 99;
	
	if(size == 2){
		for(i=0;i<(N/2)+1;i++){	
			MPI_Irecv(&vetor[aux-1], 1, MPI_INT, 0, id, interCommPai, &request[0]);
			sortPar(1);
			MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			sortImpar(1);
			MPI_Send(&vetor[aux-1], 1, MPI_INT, 0, id, interCommPai);
		}
		MPI_Send(&vetor[aux-1], (N-(aux-1))+1, MPI_INT, 0, id, interCommPai);	
	}
	else{

		for(i=0;i<(N/2)+1;i++){
			if(rank == 0){
				MPI_Irecv(&vetor[((rank+1)*aux)-1], 1, MPI_INT, 0, id, interCommPai, &request[0]);
				//printf("filho %d Irecv %d\n", rank, id);
			}else{
				MPI_Irecv(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD, &request[0]);
			//	printf("filho %d Irecv %d\n", rank, id+rank);
			}

			sortPar(rank+1);
				
			if(rank == size-2){
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			}else{
				MPI_Send(&vetor[((rank+2)*aux)-1], 1, MPI_INT, rank+1, id+rank+1, MPI_COMM_WORLD);
				MPI_Wait(&request[0], MPI_STATUS_IGNORE);
			//	printf("filho %d Send %d\n", rank, id+rank+1);
				MPI_Irecv(&vetor[((rank+2)*aux)-1], 1, MPI_INT, rank+1, id+rank+1, MPI_COMM_WORLD, &request[1]);
			//	printf("filho %d Irecv %d\n", rank, id+rank+1);
			}

			sortImpar(rank+1);
			
			if(rank == 0){
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, 0, id, interCommPai);
				MPI_Wait(&request[1], MPI_STATUS_IGNORE);
			//	printf("filho %d Send %d\n", rank, id);
			}else if(rank == size-2){
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD);
			//	printf("filho %d Send %d\n", rank, id+rank);
			}else{
				MPI_Send(&vetor[((rank+1)*aux)-1], 1, MPI_INT, rank-1, id+rank, MPI_COMM_WORLD);
				MPI_Wait(&request[1], MPI_STATUS_IGNORE);
			//	printf("filho %d Send %d\n", rank, id+rank);
			}
		}	
		if(rank == size-2){
			MPI_Send(&vetor[((rank+1)*aux)-1], aux+1, MPI_INT, 0, id+(rank*100), interCommPai);
			printf("filho %d\tvector[%ld]\taux: %ld\t Send: %d\n", rank, ((rank+1)*aux)-1, aux+1, id+(rank*100));
		}else{
			MPI_Send(&vetor[((rank+1)*aux)-1], aux, MPI_INT, 0, id+(rank*100), interCommPai);

			printf("filho %d\tvector[%ld]\taux: %ld\t Send: %d\n", rank, ((rank+1)*aux)-1, aux, id+(rank*100));
		}
	}
}

int main(int argc, char **argv){

	argv+=1;
	#ifdef PAPI
                char *papi_hex_code = argv[3];
                int PAPI_CODE = (int)strtol(papi_hex_code, NULL, 16); // get the code of a PAPI event
                int EventSet = PAPI_NULL;
                long long event_result;
                PAPI_library_init(PAPI_VER_CURRENT);
                PAPI_create_eventset(&EventSet);
                PAPI_add_event(EventSet, PAPI_CODE);
                PAPI_start(EventSet);
                PAPI_reset(EventSet);
        #endif

	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_get_parent(&interCommPai);
	

	//int i;
	//for(i=0;i<7;i++)
	//	printf("arg %d = %s\n", i, argv[i]);

	N = atoi(argv[2]);
		
	int numFilhos = atoi(argv[4]);
	size = numFilhos+1;
	vetIndice = malloc(sizeof(long int)*size*4);
	vetor = malloc(sizeof(long int)*N);
	leEntrada();
	MPI_Recv(&vetIndice[0], size*4, MPI_INT, 0, 99, interCommPai, MPI_STATUS_IGNORE);
		
	//printf("RQRQRQRQRQRQR\n");
	bubbleSort(rank);

	MPI_Finalize();

        #ifdef PAPI
                PAPI_read(EventSet, &event_result);
                printf("FILHO\tPAPI EVENT\t%s\tVALUE\t%llu\n", papi_hex_code, event_result);
                PAPI_stop(EventSet, NULL);
        #endif
	return 0;
}


#include <mpi.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int N, iter_max = 1000, indice;

void ajustaChunk2(int size, int *vetIni2, int *vetFim2){
    int i, j, chunk, chunkPlus, sobra;
    if( (N-2)%size == 0){
        chunk = (N-2)/size;
        for(i=0;i<size;i++){
                vetIni2[i] = (i*chunk)+1;
                vetFim2[i] = ((i*chunk)+1)+chunk;
        }
    }else{
        chunk = (N-2)/size;
        chunkPlus = chunk+1;
        sobra = (N-2)%size;
        for(i=0;i<size;i++){
            if(i < sobra){
                vetIni2[i] = (i*chunkPlus)+1;
                vetFim2[i] = ((i*chunkPlus)+1)+chunkPlus;
            }else{
                vetIni2[i] = vetFim2[i-1];
                vetFim2[i] = vetIni2[i]+chunk;
            }
        }
    }
}

void jacobi(int rank, int size, int *vetIni2, int *vetFim2, float *A, float *Anew, float *y){

	int i, j, iter, source;
	float *aux;
	float pi = 2.0f * asinf(1.0f);
	MPI_Request request[size+2];
	for(i=0;i<N;i++){
		A[i]   = 0.f;
        A[((N-1)*N)+i] = 0.f;
        y[i] = sinf(pi * i / (N-1));
        A[(i*N)] = y[i];
        A[(i*N)+N-1] = y[i]*expf(-pi);        
        Anew[i] = 0.f;
        Anew[((N-1)*N)+i] = 0.f;
        Anew[i*N] = y[i];
        Anew[(i*N)+N-1] = y[i]*expf(-pi);
	}

	for(iter=0;iter<iter_max;iter++){
			
		if(rank == 0){
			for(i=1;i<size;i++){
				MPI_Irecv(&Anew[vetIni2[i]*N], (vetFim2[i]-vetIni2[i])*N, MPI_FLOAT, i, 99, MPI_COMM_WORLD, &request[i-1]);
			}
		}

		for(j=vetIni2[rank]; j < vetFim2[rank]; j++){
			for(i=1;i<N-1;i++){
				Anew[(j*N)+i] = 0.25f * ( A[(j*N)+i+1] + A[(j*N)+i-1]+ A[((j-1)*N)+i] + A[((j+1)*N)+i]);
			}
		}
		if(rank == 0){
				
			for(i=0;i<size-1;i++){
				MPI_Waitany(size-1, request, &source, MPI_STATUS_IGNORE);
			}
			for(i=1;i<size;i++){
				MPI_Send(&Anew[0], N*N, MPI_FLOAT, i, 199, MPI_COMM_WORLD);
			}
		}else{
			MPI_Irecv(&Anew[0], N*N, MPI_FLOAT, 0, 199, MPI_COMM_WORLD, &request[0]);
			MPI_Send(&Anew[vetIni2[rank]*N], (vetFim2[rank]-vetIni2[rank])*N, MPI_FLOAT, 0, 99, MPI_COMM_WORLD);
			MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}

		aux = A;
		A = Anew;
		Anew = aux;
	}
}


int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size>\n\n \tE.g. -> mpirun -np 2 ./my_program 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int pid;
	N = atoi(argv[1]);
	int rank, size, i, j;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	float *A, *Anew, *y;
	int *vetFim2, *vetIni2;
	
	A = malloc(sizeof(float)*N*N);
	Anew = malloc(sizeof(float)*N*N);
	y = malloc(sizeof(float)*N);

	vetIni2 = malloc(sizeof(int)*size);
	vetFim2 = malloc(sizeof(int)*size);

	ajustaChunk2(size, vetIni2, vetFim2);

	jacobi(rank, size, vetIni2, vetFim2, A, Anew, y);
	/*
	if(rank == 0){
   		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
					printf("%f ", A[(i*N)+j]);
			}
			printf("\n");
		}
	}*/
	MPI_Finalize();
		
	if (rank == 0){
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Excution time\t%f\n", delta);
		#endif
	}
	return 0;
}

#include <mpi.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int N, iter_max = 1000;

MPI_Comm intercommFilho[128];

void ajustaChunk2(int size, int *vetIni, int *vetFim){
    int i, j, chunk, chunkPlus, sobra;
    if((N-2)%size == 0){
        chunk = (N-2)/size;
        for(i=0;i<size;i++){
            vetIni[i] = (i*chunk)+1;
            vetFim[i] = ((i*chunk)+1)+chunk;
        }
    }else{
        chunk = (N-2)/size;
        chunkPlus = chunk+1;
        sobra = (N-2)%size;
        for(i=0;i<size;i++){
            if(i < sobra){
                vetIni[i] = (i*chunkPlus)+1;
                vetFim[i] = ((i*chunkPlus)+1)+chunkPlus;
            }else{
                vetIni[i] = vetFim[i-1];
                vetFim[i] = vetIni[i]+chunk;
            }
        }
    }
}

void jacobi(int size, int *vetIni, int *vetFim, float *A, float *Anew, float *y){
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
		for(i=0;i<size;i++){
			MPI_Irecv(&Anew[vetIni[i+1]*N], (vetFim[i+1]-vetIni[i+1])*N, MPI_FLOAT, 0, 99, intercommFilho[i], &request[i]);
		}				

		for(j=vetIni[0]; j < vetFim[0]; j++){
			for(i=1;i<N-1;i++){
				Anew[(j*N)+i] = 0.25f * ( A[(j*N)+i+1] + A[(j*N)+i-1]+ A[((j-1)*N)+i] + A[((j+1)*N)+i]);
			}
		}
		
		for(i=0;i<size;i++){
			MPI_Waitany(size, request, &source, MPI_STATUS_IGNORE);
		}
		for(i=0;i<size;i++){
			MPI_Send(&Anew[0], N*N, MPI_FLOAT, 0, 99, intercommFilho[i]);
		}
		aux = A;
		A = Anew;
		Anew = aux;
	}
}

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 2048 \"$PWD/child\"\n\n");
		exit(1);
	}
	
	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	
	int pid;
	MPI_Init(&argc, &argv);
	int err[1];
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	
	int numProc = atoi(argv[1]);
	int i, j;
	float *A, *Anew, *y;
	int *vetFim, *vetIni;
	
	N = atoi(argv[2]);
	
	char *bin = realpath(argv[argc-1], NULL); //find the child full path

	A = malloc(sizeof(float)*N*N);
	Anew = malloc(sizeof(float)*N*N);
	y = malloc(sizeof(float)*N);

	vetIni = malloc(sizeof(int)*numProc+1);
	vetFim = malloc(sizeof(int)*numProc+1);

	ajustaChunk2(numProc+1, vetIni, vetFim);

	for(i=argc; i<10;i++){
		argv[i] = (char*) malloc(sizeof(char)*10);
	}
	
	for(i=0;i<numProc;i++){
		sprintf(argv[3], "%d", vetIni[i+1]);
		sprintf(argv[4], "%d", vetFim[i+1]);
		argv[5] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &intercommFilho[i], err);
	}

	jacobi(numProc, vetIni, vetFim, A, Anew, y);

	/*for(i=0;i<N;i++){
			for(j=0;j<N;j++){
					printf("%f ", A[(i*N)+j]);
			}
			printf("\n");
	}*/

	MPI_Finalize();
	
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

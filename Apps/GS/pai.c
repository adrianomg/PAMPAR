#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <sys/time.h>

int N, *vetIni, *vetFim, *vetChunk, *matIni, *matFim;
float *Q, *Qt;
MPI_Comm interCommPai[128];

void checkOctave(){
	int found_error=0;
	int c1, c2, i;
	float sum;
	for(c1=0; c1 < N; c1++){
		for(c2=c1; c2 < N; c2++) {
			sum=0.;
			for(i=0; i < N; i++) 
				sum += Q[(i*N)+c1] * Q[(i*N)+c2];
			if(c1 == c2) { // should be near 1 (unit length)
				if(sum < 0.9) {
					printf("Failed unit length: %d %d %g\n", c1,c2,sum);
					found_error = 1;
					exit(1);
				}
			}else{ // should be very small (orthogonal)
				if(sum > 0.1) {
					printf("Failed orthonogonal  %d %d %g\n", c1,c2,sum);
					found_error = 1;
					exit(1);
				}
			}
		}
	}
	if(!found_error) printf("Check OK!\n");
}


void ajustaChunk(int size){
	int i, j, chunk, chunkPlus, sobra;
	if(N%size == 0){
		chunk = N/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
			vetChunk[i] = vetFim[i] - vetIni[i];
		}
	}else{
		chunk = N/size;
		chunkPlus = chunk+1;
		sobra = N%size;
		for(i=0;i<size;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
			}
			vetChunk[i] = vetFim[i] - vetIni[i];
		}
	}
}

void gramschmidt(int numProc){
	int i, j, k, source;
	float tmp_local=0, tmp_aux[numProc];
	float tmp=0;
	MPI_Request request[numProc+1];

	for(k=0;k<N;k++){
		tmp = 0;
		tmp_local = 0;
		
		for(i=0;i<numProc;i++){
			tmp_aux[i] = 0.0;
			MPI_Irecv(&tmp_aux[i], 1, MPI_FLOAT, 0, 99, interCommPai[i], &request[i]);
		}

		for(i=vetIni[0]; i< vetFim[0]; i++){
				tmp += (Q[(i*N)+k] * Q[(i*N)+k]);
		}
		for(i=0;i<numProc;i++){
			MPI_Recv(&tmp_aux, 1, MPI_FLOAT, 0, 99, interCommPai[i], MPI_STATUS_IGNORE);
			MPI_Waitany(numProc, request, &source, MPI_STATUS_IGNORE);
			tmp += tmp_aux[source];
		}
		tmp = sqrt(tmp);

		for(i=0;i<numProc;i++){
			MPI_Send(&tmp,1, MPI_FLOAT, 0, 99, interCommPai[i]);
		}

		for(i=0; i<N; i++){
			Q[(i*N)+k] /= tmp;
		}


		for(j=k+1; j<N;j++){
			tmp = 0;
			for(i=0;i<numProc;i++){
				MPI_Irecv(&tmp_aux[i], 1, MPI_FLOAT, 0, 99, interCommPai[i], &request[i]);
			}
			for(i=vetIni[0]; i < vetFim[0]; i++){
				tmp += Q[(i*N)+k] * Q[(i*N)+j];
			}
			for(i=0;i<numProc;i++){
				MPI_Recv(&tmp_aux, 1, MPI_FLOAT, 0, 99, interCommPai[i], MPI_STATUS_IGNORE);
				MPI_Waitany(numProc, request, &source, MPI_STATUS_IGNORE);
				tmp += tmp_aux[source];
			}
			for(i=0;i<numProc;i++){
				MPI_Send(&tmp,1, MPI_FLOAT, 0, 99, interCommPai[i]);
			}
			for(i=0; i < N; i++){
				Q[(i*N)+j] -= tmp * Q[(i*N)+k];
			}
		}
	}
}

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-proc>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 2048 "$PWD/child"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	
	MPI_Init(&argc, &argv);
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	int err[1];
	int numProc = atoi(argv[1]);
	
	N = atoi(argv[2]);
	char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);

	vetIni = malloc(sizeof(int)*numProc+1);
	vetFim = malloc(sizeof(int)*numProc+1);
	vetChunk = malloc(sizeof(int)*numProc+1);
	Q = malloc(sizeof(float)*N*N);
	int i, j;

	ajustaChunk(numProc+1);

	for(i=argc; i <6;i++){
		argv[i] = malloc(sizeof(char)*10);
	}

	for(i=0; i < N; i++)
		for(j=0; j < N; j++)
			Q[(i*N)+j] = (float)rand()/ (float)RAND_MAX;

	for(i=0;i<numProc;i++){
		sprintf(argv[3], "%d", vetIni[i+1]);
		sprintf(argv[4], "%d", vetFim[i+1]);
		argv[5] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &interCommPai[i], err);
		MPI_Send(&Q[0], N*N, MPI_FLOAT, 0, 99, interCommPai[i]);
	}

	gramschmidt(numProc);
	//checkOctave();

	free(Q);
	MPI_Finalize();

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
}

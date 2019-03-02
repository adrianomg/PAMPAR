#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

int N, indice, size, rank, *vetIni, *vetFim, *vetChunk, *matIni, *matFim;
float *Q, *Qt;

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

void ajustaChunk(){
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

void gramschmidt(){
	int i, j, k;
	float tmp_local=0;
	float tmp=0;

	for(k=0;k<N;k++){
		tmp = 0;
		tmp_local = 0;
		for(i=vetIni[rank]; i< vetFim[rank]; i++){
			tmp_local += (Q[(i*N)+k] * Q[(i*N)+k]);
		}
		MPI_Allreduce(&tmp_local, &tmp, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD); //Já foi testado comunicações assincronas, porém, a melhor foi essa!
		tmp = sqrt(tmp);
		for(i=0; i<N; i++){
			Q[(i*N)+k] /= tmp;
		}
		for(j=k+1; j<N;j++){
			tmp = 0;
			tmp_local = 0;
			for(i=vetIni[rank]; i < vetFim[rank]; i++){
					tmp_local += Q[(i*N)+k] * Q[(i*N)+j];
			}
			MPI_Allreduce(&tmp_local, &tmp, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);	//Já foi testado comunicações assincronas, porém, a melhor foi essa!
			for(i=0; i < N; i++){
					Q[(i*N)+j] -= tmp * Q[(i*N)+k];
			}
		}
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
    
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	N = atoi(argv[1]);

	vetIni = malloc(sizeof(int)*size);
	vetFim = malloc(sizeof(int)*size);
	vetChunk = malloc(sizeof(int)*size);
	Q = malloc(sizeof(float)*N*N);
	int i, j;

	if(rank == 0){
		for(i=0; i < N; i++)
			for(j=0; j < N; j++)
				Q[(i*N)+j] = (float)rand()/ (float)RAND_MAX;
	}
	MPI_Bcast(&Q[0], N*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
	ajustaChunk();

	gramschmidt();
	//checkOctave();

	free(Q);
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

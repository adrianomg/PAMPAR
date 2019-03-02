#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>
#define infinity 99999999

int indice, *G, vertices, *vetIni, *vetFim;

void dijkstra(int rank, int size){
	int *dis, i, t, v, *vis, source;
	dis = malloc(sizeof(int)*vertices);
	vis = malloc(sizeof(int)*vertices);
	for(source = vetIni[rank];source<vetFim[rank];source++){
		for(i=0;i<vertices;i++){
			dis[i] = infinity;
			vis[i] = 0;
		}
		dis[source] = 0;
		for(t=0;t<vertices;t++){
			v = -1;
			for(i=0;i<vertices;i++){
				if(!vis[i] && (v<0 || dis[i] < dis[v])){
					v = i;
				}
			}
			vis[v] = 1;
			for(i=0;i<vertices;i++){
				if(G[(v*vertices)+i] && dis[i] > dis[v] + G[(v*vertices)+i]){
						dis[i] = dis[v] + G[(v*vertices)+i];
				}
			}
		}		
	} 

	if(rank == 0){
		for(i=1;i<size;i++){
			MPI_Recv(&dis[vetIni[i]], (vetFim[i]-vetIni[i]), MPI_INT, i, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}
	else{
		MPI_Send(&dis[vetIni[rank]], (vetFim[rank] - vetIni[rank]), MPI_INT, 0, 99, MPI_COMM_WORLD);
	}
}

void preencheGrafo(FILE *input){
	int i, j;
	for(i=0;i<vertices;i++){
		for(j=0;j<vertices;j++){
			fscanf(input, "%d", &G[(i*vertices)+j]);
			if(G[(i*vertices)+j] == -1){
				G[(i*vertices)+j] = infinity;
			}
		}
	}
}

void ajustaChunk(int size){
	int i, j, chunk, chunkPlus, sobra;
	if(vertices%size == 0){
		chunk = vertices/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = vertices/size;
		chunkPlus = chunk+1;
		sobra = vertices%size;
		for(i=0;i<size;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
			}
		}
	}
}

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-size> <input-file> \n\n \tE.g. -> mpirun -np 2 ./my_program 2048 2048.txt\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int i;
	FILE *input;
	vertices = atoi(argv[1]);
	input = fopen(argv[2], "r");
	
	G = malloc(sizeof(int)*vertices*vertices);
	vetIni = malloc(sizeof(int)*size);
	vetFim = malloc(sizeof(int)*size);
	
	ajustaChunk(size);
	
	if(rank == 0){
		preencheGrafo(input);
	}
	MPI_Bcast(&G[0], (vertices*vertices), MPI_INT, 0, MPI_COMM_WORLD);

	dijkstra(rank, size);
	
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

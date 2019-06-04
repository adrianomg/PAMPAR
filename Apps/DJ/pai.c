#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>
#define infinity 99999999

int indice, *G, vertices, *vetIni, *vetFim, *dis;

void dijkstra(int rank){
	int i, t, v, *vis, source;
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

	if (argc < 5) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <input-size> <input-file> <n-child-proc> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 2048 2048.txt 3 \"$PWD/child\"\n\n");
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
	int err[1];
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
		
	char *bin;
	int tam1 = strlen(argv[argc-1]);
	bin = (char*)malloc((tam1)*sizeof(char));
	strcpy(bin, argv[argc-1]);

	int i;
	FILE *input;
	vertices = atoi(argv[1]);
	input = fopen(argv[2], "r");
	int num_Proc = atoi(argv[3]);
	MPI_Comm intercommFilho[num_Proc];
		
	G = malloc(sizeof(int)*(vertices*vertices));
	vetIni = malloc(sizeof(int)*num_Proc+1);
	vetFim = malloc(sizeof(int)*num_Proc+1);
		
	ajustaChunk(num_Proc+1);
		
	preencheGrafo(input);
		
	for(i=argc;i<=10;i++){
		argv[i] = (char*) malloc(sizeof(char)*10);
	}
	for(i=0;i<num_Proc;i++){
		sprintf(argv[5], "%d", vetIni[i+1]);
		sprintf(argv[6], "%d", vetFim[i+1]);
		sprintf(argv[7], "%d", vertices);
		argv[8] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, rank, MPI_COMM_SELF, &intercommFilho[i], err);
		MPI_Send(&G[0], (vertices*vertices), MPI_INT, 0, 99, intercommFilho[i]);
	}

	dijkstra(rank);
		
	for(i=0;i<num_Proc;i++){
		MPI_Recv(&dis[vetIni[i+1]], (vetFim[i+1]-vetIni[i+1]), MPI_INT, 0, 99, intercommFilho[i], MPI_STATUS_IGNORE);
	}
	MPI_Finalize();
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
			
	return 0;
}		

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>
int vertices, *G;
#define infinity 99999999

void dijkstra(int source){
	int i, v, t;
	#pragma omp parallel private(source, i, v, t)
	{
		int *dis, *vis;
		dis = malloc(sizeof(int)*vertices);
		vis = malloc(sizeof(int)*vertices);
		#pragma omp for schedule(static)
		for(source=0;source<vertices;source++){
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

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: my_program <threads> <input-size> <input-file> \n\n \tE.g. -> ./my_program 2048 2048.txt 2\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int arestas, source;
	FILE *input;
	
	vertices = atoi(argv[2]);
	input = fopen(argv[3], "r");
	omp_set_num_threads(atoi(argv[1]));

	G = malloc(sizeof(int)*vertices*vertices);

	preencheGrafo(input);

	dijkstra(source);
        
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

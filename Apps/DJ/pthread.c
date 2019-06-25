#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#define infinity 99999999

long int num_threads;
int indice, *G, vertices, *vetIni, *vetFim;
pthread_mutex_t mutexSum;

void dijkstra(long int idThread){
	int *dis, i, t, v, *vis, source;
	dis = malloc(sizeof(int)*vertices);
	vis = malloc(sizeof(int)*vertices);
	for(source = vetIni[idThread-1];source<vetFim[idThread-1];source++){
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

void *djikstra_pthread(void *threadId){
		long int idThread = (long int) threadId;
		dijkstra(idThread);
		return NULL;
}

void ajustaChunk(){
	int i, j, chunk, chunkPlus, sobra;
	if(vertices%num_threads == 0){
		chunk = vertices/num_threads;
		for(i=0;i<num_threads;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = vertices/num_threads;
		chunkPlus = chunk+1;
		sobra = vertices%num_threads;
		for(i=0;i<num_threads;i++){
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
	
	if (argc < 4) {
		printf ("ERROR! Usage: my_program <threads> <input-size> <input-file> \n\n \tE.g. -> ./my_program 2048 2048.txt 2\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	long int i;
	FILE *input;

	num_threads = atol(argv[1]);
	vertices = atoi(argv[2]);
	input = fopen(argv[3], "r");
	
	pthread_t threads[num_threads];

	G = malloc(sizeof(int)*vertices*vertices);
	vetIni = malloc(sizeof(int)*num_threads);
	vetFim = malloc(sizeof(int)*num_threads);

	preencheGrafo(input);

	ajustaChunk();

	for(i=1; i<= num_threads; i++){
		pthread_create(&threads[i], NULL, djikstra_pthread, (void *)i);
	}
	for(i=1;i<= num_threads; i++){
		pthread_join(threads[i], NULL);
	}
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#define infinity 99999999

int indice, *G, vertices, ini, fim, *dis;

void dijkstra(int rank){
		int i, t, v, *vis, source;
		dis = malloc(sizeof(int)*vertices);
		vis = malloc(sizeof(int)*vertices);
		for(source = ini;source<fim;source++){
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


int main(int argc, char **argv){

		argv+=1;
		int rank, size;
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm intercommPai;
        	MPI_Comm_get_parent(&intercommPai);

		int i;

		ini = atoi(argv[5]);
		fim = atoi(argv[6]);				
		vertices = atoi(argv[7]);
		G = malloc(sizeof(int)*vertices*vertices);

		MPI_Recv(&G[0], (vertices*vertices), MPI_INT, 0, 99, intercommPai, MPI_STATUS_IGNORE);
		dijkstra(rank);
		
		MPI_Send(&dis[ini], fim-ini, MPI_INT, 0, 99, intercommPai);
		
		MPI_Finalize();
		return 0;
}		

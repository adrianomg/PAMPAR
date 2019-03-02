#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int hmax, wmax;

float alvo[256], *matriz;
int lx=20, ly=20, *vetIni, *vetFim, linhas_validas;

void histograma(int hc, int wc, int hl, int wl, float hist[]){
        int h,w,np,i;  
        for (i=0;i<=255;i++){
                hist[i]=0;
        }
        for (h=hc-hl;h<hc+hl;h++){
                for (w=wc-wl;w<wc+wl;w++){
                        hist[(int)matriz[(h*wmax)+w]]=hist[(int)matriz[(h*wmax)+w]]+1;
                }
        }
        np=(2*hl+1)*(2*wl+1);
        for (i=0;i<=255;i++){
                   hist[i]=hist[i]/np;
        }
}

void distbhat(float hist[], float alvo[],float *db){
        int i;
        float coefb;
        coefb=0.0;
        for (i=0;i<=255;i++){
                coefb=coefb+sqrt(hist[i]*alvo[i]);
        }
        *db=sqrt(1.0-coefb);
}

void ajustaChunk(int size){
		int i, j, chunk, chunkPlus, sobra;
		if(linhas_validas%size == 0){
			chunk = linhas_validas/size;
			for(i=0;i<size;i++){
					vetIni[i] = (i*chunk)+lx;
					vetFim[i] = vetIni[i]+chunk;
			}
		}else{
			chunk = linhas_validas/size;
			chunkPlus = chunk+1;
			sobra = linhas_validas%size;
			for(i=0;i<size;i++){
					if(i < sobra){
						vetIni[i] = (i*chunkPlus)+lx;
						vetFim[i] = vetIni[i]+chunkPlus;
					}else{
						vetIni[i] = vetFim[i-1];
						vetFim[i] = vetIni[i]+chunk;
					}
			}
		}
}

int main(int argc, char **argv){
    	
    	if (argc < 4) {
			printf ("ERROR! Usage: mpirun -np <n-proc> my_program <input-image> <image-height> <image-width>\n\n \tE.g. -> mpirun -np 2 ./my_program image.txt 1920 1080\n\n");
			exit(1);
		}
    	
    	#ifdef ELAPSEDTIME
			struct timeval start, end;
			gettimeofday(&start, NULL);
		#endif
		int size, rank, h, w, i;
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
        
        hmax = atoi(argv[2]);
        wmax = atoi(argv[3]);
        
		FILE *input;
		float *matrizOut;
		float hist[256], db;
		matriz = malloc(sizeof(float)*hmax*wmax);
		matrizOut = malloc(sizeof(float)*hmax*wmax);
		if(rank == 0){
				input = fopen(argv[1], "r");
				for(h=0;h<hmax;h++){
						for(w=0;w<wmax;w++){
								fscanf(input, "%f", &matriz[(h*wmax)+w]);
						}
				}				
		}

		MPI_Bcast(&matriz[0], hmax*wmax, MPI_FLOAT, 0, MPI_COMM_WORLD);
		if(rank == 0)
				histograma(310, 714, lx, ly, alvo);
		
		MPI_Bcast(&alvo[0], 256, MPI_FLOAT, 0, MPI_COMM_WORLD);
		vetIni = malloc(sizeof(int)*size);
		vetFim = malloc(sizeof(int)*size);
		linhas_validas = hmax-(lx+ly);
		ajustaChunk(size);
		for(h=vetIni[rank]; h<vetFim[rank]; h++){
				for(w=0;w<wmax-ly; w++){
						histograma(h, w, lx, ly, hist);
						distbhat(alvo, hist, &db);
						matrizOut[(h*wmax)+w] = round(255*db);
				}
		}
		if(rank == 0){
				for(i=1;i<size;i++){
						MPI_Recv(&matrizOut[vetIni[i]*wmax], ((vetFim[i]-vetIni[i])*wmax), MPI_FLOAT, i, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}
		}else{
				MPI_Send(&matrizOut[vetIni[rank]*wmax], ((vetFim[rank]-vetIni[rank])*wmax), MPI_FLOAT, 0, 99, MPI_COMM_WORLD);
		}

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

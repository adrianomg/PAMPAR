#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int hmax, wmax;
float alvo[256];
int lx=20, ly=20, *vetIni, *vetFim, linhas_validas;
float *matriz;

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

		if (argc < 6) {
			printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-image> <image-height> <image-width> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program image.txt 3 1920 1080 \"$PWD/child\"\n\n");
			exit(1);
		}

        #ifdef ELAPSEDTIME
			struct timeval start, end;
			gettimeofday(&start, NULL);
		#endif
		int size, rank, h, w, i;
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		int err[1];
        MPI_Info localInfo;
        MPI_Info_create(&localInfo);

        size = atoi(argv[1])+1;
        MPI_Comm intercommFilho[size];

		
		hmax = atoi(argv[3]);
		wmax = atoi(argv[4]);
		char *bin;
		int tam1 = strlen(argv[argc-1]);
		bin = (char*)malloc((tam1)*sizeof(char));
		strcpy(bin, argv[argc-1]);

		
		FILE *input;
		float *matrizOut;
		float hist[256], db;
		matriz = malloc(sizeof(float)*hmax*wmax);
		matrizOut = malloc(sizeof(float)*hmax*wmax);
		input = fopen(argv[2], "r");
		for(h=0;h<hmax;h++){
				for(w=0;w<wmax;w++){
						fscanf(input, "%f", &matriz[(h*wmax)+w]);
				}
		}				

		for(i=argc-1;i<10;i++){
				argv[i] = (char*) malloc(sizeof(char)*10);
		}

		vetIni = malloc(sizeof(int)*size);
		vetFim = malloc(sizeof(int)*size);
		linhas_validas = hmax-(lx+ly);
		ajustaChunk(size);

		for(i=0;i<size-1;i++){
				sprintf(argv[5], "%d", vetIni[i+1]);
				sprintf(argv[6], "%d", vetFim[i+1]);
				MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &intercommFilho[i], err);
				MPI_Send(&matriz[0], hmax*wmax, MPI_FLOAT, 0, 99, intercommFilho[i]);
		}
		histograma(310, 714, lx, ly, alvo);
		for(i=0;i<size-1;i++)
				MPI_Send(&alvo[0], 256, MPI_FLOAT, 0, 99, intercommFilho[i]);

		for(h=vetIni[0]; h<vetFim[0]; h++){
				for(w=0;w<wmax-ly; w++){
						histograma(h, w, lx, ly, hist);
						distbhat(alvo, hist, &db);
						matrizOut[(h*wmax)+w] = round(255*db);
				}
		}

		for(i=0;i<size-1;i++){
				MPI_Recv(&matrizOut[vetIni[i+1]*wmax], ((vetFim[i+1]-vetIni[i+1])*wmax), MPI_FLOAT, 0, 99, intercommFilho[i], MPI_STATUS_IGNORE);
		}

		MPI_Finalize();
		
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Execution time\t%f\n", delta);
		#endif
		
		return 0;
}

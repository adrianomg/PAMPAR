#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int hmax, wmax;

pthread_mutex_t mutexSum;
float *matriz, *matrizImg, alvo[256];
int lx=20, ly=20, *vetIni, *vetFim, linhas_validas, num_threads, indice;

void histograma(int hc, int wc, int hl, int wl, float hist[]){
        int h,w,np,i;
        for (i=0;i<=255;i++){
                hist[i]=0;
        }
        for (h=hc-hl;h<hc+hl;h++){
                for (w=wc-wl;w<wc+wl;w++){
                        hist[(int)matrizImg[(h*wmax)+w]]=hist[(int)matrizImg[(h*wmax)+w]]+1;
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

void ajustaChunk(){
		int i, j, chunk, chunkPlus, sobra;
		if(linhas_validas%num_threads == 0){
			chunk = linhas_validas/num_threads;
			for(i=0;i<num_threads;i++){
					vetIni[i] = (i*chunk)+lx;
					vetFim[i] = vetIni[i]+chunk;
			}
		}else{
			chunk = linhas_validas/num_threads;
			chunkPlus = chunk+1;
			sobra = linhas_validas%num_threads;
			for(i=0;i<num_threads;i++){
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

void *funcao_thread(void *arg){
		long int idThread = (long int) arg;
		int h, w;
		float hist[256], db;
		for(h=vetIni[idThread]; h<vetFim[idThread]; h++){
				for(w=ly; w < wmax-ly; w++){
						histograma(h, w, lx, ly, hist);
						distbhat(alvo, hist, &db);
						matriz[(h*wmax)+w] = round(255*db);
				}
		}
}

int main(int argc, char **argv){

		if (argc < 5) {
			printf ("ERROR! Usage: my_program <input-image> <image-height> <image-width> <threads>\n\n \tE.g. -> ./my_program image.txt 1920 1080 2\n\n");
			exit(1);
		}

		#ifdef ELAPSEDTIME
			struct timeval start, end;
			gettimeofday(&start, NULL);
		#endif

		FILE *input, *output;
		hmax = atoi(argv[2]);
		wmax = atoi(argv[3]);
		input = fopen(argv[1], "r");
		output = fopen("saida.txt", "w");
		long int i;
		int h, w;
		matriz = malloc(sizeof(float)*hmax *wmax);
		matrizImg = malloc(sizeof(float)*hmax *wmax);
		for(h=0;h<hmax;h++){
				for(w=0;w<wmax;w++){
						fscanf(input, "%f", &matrizImg[(h*wmax)+w]);
				}
		}
		histograma(310, 714, lx, ly, alvo);
		num_threads = atoi(argv[4]);
		pthread_t threads[num_threads];
		vetIni = (int*) malloc(sizeof(int)*num_threads);
		vetFim = (int*)malloc(sizeof(int)*num_threads);
		linhas_validas = hmax-(lx+lx);
		ajustaChunk();

		for(i=0;i<num_threads;i++){
				pthread_create(&threads[i], NULL, funcao_thread, (void *) i);
		}
		for(i=0;i<num_threads;i++){
				pthread_join(threads[i], NULL);
		}
		
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Execution time\t%f\n", delta);
		#endif
		
		return 0;
}

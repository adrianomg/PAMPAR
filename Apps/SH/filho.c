#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int hmax, wmax;

float alvo[256], *matriz;
int lx=20, ly=20;

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

int main(int argc, char **argv){
		argv+=1;
        MPI_Init(&argc, &argv);
        MPI_Comm intercommPai;
        MPI_Comm_get_parent(&intercommPai);
        
		hmax = atoi(argv[3]);
		wmax = atoi(argv[4]);

        int ini = atoi(argv[5]);
        int fim = atoi(argv[6]);

        matriz = malloc(sizeof(float)*hmax*wmax);
        float *matrizOut = malloc(sizeof(float)*hmax*wmax);
        float hist[256], db;
       
        MPI_Recv(&matriz[0], hmax*wmax, MPI_FLOAT, 0, 99, intercommPai, MPI_STATUS_IGNORE);
        MPI_Recv(&alvo[0], 256, MPI_FLOAT, 0, 99, intercommPai, MPI_STATUS_IGNORE);

        int h, w;
        for(h=ini;h<fim;h++){
                for(w=0;w<wmax-ly; w++){
                        histograma(h, w, lx, ly, hist);
                        distbhat(alvo, hist, &db);
                        matrizOut[(h*wmax)+w] = round(255*db);
                }
        }

        MPI_Send(&matrizOut[ini*wmax], ((fim-ini)*wmax), MPI_FLOAT, 0, 99, intercommPai);

        MPI_Finalize();
        return 0;
}

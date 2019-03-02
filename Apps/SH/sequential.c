#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

void histograma(float *img, int hc, int wc, int hl, int wl, float hist[], int hmax, int wmax){
        int h,w,np,i;
        for (i=0;i<=255;i++){
                hist[i]=0;
        }
        int j=0;
        for (h=hc-hl;h<=hc+hl;h++){
                for (w=wc-wl;w<=wc+wl;w++){
                        hist[(int)img[(h*wmax)+w]]=hist[(int)img[(h*wmax)+w]]+1;
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

		if (argc < 4) {
			printf ("ERROR! Usage: my_program <input-image> <image-height> <image-width>\n\n \tE.g. -> ./my_program image.txt 1920 1080\n\n");
			exit(1);
		}

		#ifdef ELAPSEDTIME
			struct timeval start, end;
			gettimeofday(&start, NULL);
		#endif
		
        int h,w,l,c,i,lx,ly,p[3];
        float alvo[256],hist[256],soma,db, *img, *imgout;
        FILE *input = fopen(argv[1], "r");
        int hmax = atoi(argv[2]);
        int wmax = atoi(argv[3]);
        img = malloc(sizeof(float)*hmax*wmax);
		imgout = malloc(sizeof(float)*hmax*wmax);
		for(h=0;h<hmax;h++){
				for(w=0;w<wmax;w++){
						fscanf(input, "%f", &img[(h*wmax)+w]);
				}
		}
		lx=20;
        ly=20;
        
		histograma(img, 216, 138, lx, ly, alvo, hmax, wmax);
        for(h=0+lx+1;h<=hmax-lx-1;h++){
                for (w=0+ly+1;w<=wmax-ly-1;w++){
                        histograma(img, h, w, lx, ly, hist, hmax, wmax);
                        distbhat(hist,alvo,&db);
                        imgout[(h*wmax)+w]=round(255*db);
                }
        }
        
		#ifdef ELAPSEDTIME
			gettimeofday(&end, NULL);
			double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
			printf("Excution time\t%f\n", delta);
		#endif
        return 0;
}



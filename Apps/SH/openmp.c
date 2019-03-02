#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

int hmax, wmax;

void histograma(float *img, int hc, int wc, int hl, int wl, float hist[]){
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

	if (argc < 5) {
		printf ("ERROR! Usage: my_program <input-image> <image-height> <image-width> <threads>\n\n \tE.g. -> ./my_program image.txt 1920 1080 2\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    int h,w,l,c,i,lx,ly,p[3], chunk = 1;
    float alvo[256],hist[256],soma,db;
	FILE *input, *output;
	hmax = atoi(argv[2]);
	wmax = atoi(argv[3]);
	input = fopen(argv[1], "r");
	output = fopen("saida_img.txt", "wt");
    float *img = malloc(sizeof(float)*hmax*wmax);
	float *imgout = malloc(sizeof(float)*hmax*wmax);

	for(h=0;h<hmax;h++){
		for(w=0;w<wmax;w++){
			fscanf(input, "%f", &img[(h*wmax)+w]);
		}
	}

	lx=20;
    ly=20;
    histograma(img, 216, 138, lx, ly, alvo);
    omp_set_num_threads(atoi(argv[4]));

    #pragma omp parallel private ( hist,h, w, db)
    {
        #pragma omp for schedule(static)
        for (h=0+lx+1;h<=hmax-lx-1;h++){
            for (w=0+ly+1;w<=wmax-ly-1;w++){
                histograma(img, h, w, lx, ly, hist);
                distbhat(hist,alvo,&db);
                imgout[(h*wmax)+w]=round(255*db);
            }
        }
    }
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
    
    return 0;
}



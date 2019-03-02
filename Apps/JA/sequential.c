#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char** argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 2048\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    int n, m, N;
    int iter_max = 1000;
    int i, j;
    int iter = 0;

    N = atoi(argv[1]);
    m = N;
    n = N;

    const float pi  = 2.0f * asinf(1.0f);
    
    float *A = malloc(sizeof(float)*m*n);
    float *Anew = malloc(sizeof(float)*m*n);
    float *y0 = malloc(sizeof(float)*n);
    float *aux;
    memset(A, 0, n * m * sizeof(float));

    for(i = 0; i < m; i++){
        A[i]   = 0.f;
        A[((n-1)*m)+i] = 0.f;
        y0[i] = sinf(pi * i / (n-1));
        A[(i*n)] = y0[i];
        A[(i*n)+m-1] = y0[i]*expf(-pi);        
        Anew[i] = 0.f;
        Anew[((n-1)*n)+i] = 0.f;
        Anew[i*n] = y0[i];
        Anew[(i*n)+m-1] = y0[i]*expf(-pi);
    }
    for(iter=0;iter<iter_max;iter++){
        for(j = 1; j < n-1; j++){
            for(i = 1; i < m-1; i++ ){
                Anew[(j*n)+i] = 0.25f * ( A[(j*n)+i+1] + A[(j*n)+i-1]+ A[((j-1)*n)+i] + A[((j+1)*n)+i]);
            }
        }                
		aux = A;
		A = Anew;
		Anew = aux;
    }
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

        /*for(i=0;i<1;i++){
                for(j=0;j<1;j++){
                        printf("%f ", A[(i*n)+j]);
                }
                printf("\n");
        }*/

    return 0;
}

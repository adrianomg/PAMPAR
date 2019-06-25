#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
//#include <papi.h>

int main(int argc, char** argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 2048\n\n");
		exit(1);
	}
	
	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
    int m = atoi(argv[2]);
    int n = m;
    int iter_max = 1000;
    int i, j;
    int iter = 0;

    const float pi  = 2.0f * asinf(1.0f);
    
    float *A = malloc(sizeof(float)*m*n);
    float *Anew = malloc(sizeof(float)*m*n);
    float *y0 = malloc(sizeof(float)*n);
    float *aux;
    memset(A, 0, n * m * sizeof(float));

    omp_set_num_threads(atoi(argv[1]));
    int num_threads = atoi(argv[1]);
        
    int chunk = 1;

    // set boundary conditions
    #pragma omp parallel private(i)
    {
        #pragma omp for schedule(static)
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
    }

    #pragma omp parallel private(iter, i, j)
    {
        for(iter=0;iter<iter_max;iter++){
            #pragma omp for schedule(static)
            for(j = 1; j < n-1; j++){
                for(i = 1; i < m-1; i++ ){
                    Anew[(j*n)+i] = 0.25f * ( A[(j*n)+i+1] + A[(j*n)+i-1]+ A[((j-1)*n)+i] + A[((j+1)*n)+i]);
                }
            }                
            #pragma omp master
            {
                aux = A;
                A = Anew;
                Anew = aux;
            }
        }
    }
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

    return 0;
}

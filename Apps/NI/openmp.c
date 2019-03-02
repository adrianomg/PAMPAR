//C program which estimates an integral by using an averaging technique

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

double f( double x ){
    double pi = 3.141592653589793;
    double value;
    value = 50.0 / ( pi * ( 2500.0 * x * x + 1.0 ) );
    return value;
}


int main ( int argc, char *argv[] ){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 1000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

    double a = 0.0;
    double b = 10.0;
    long long int i, n;
    omp_set_num_threads(atoi(argv[1]));
	n = strtoul(argv[2], NULL, 10);
    double total = 0.0;
    double x;
    #pragma omp parallel private(x, i) reduction(+:total)
    {
		#pragma omp for schedule(static)
		for(i = 0; i < n; i++ ){
			x = ((n - i - 1) * a + (i) * b) / (n - 1);
			total = total + f(x);
		}
    }
    total = (b - a) * total / (double) n;
    
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

    //printf("%f\n", total);
    return 0;
}

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef PAPI
        #include <papi.h>
#endif

long long int NUM_PONTOS;

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 4000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	long long int i;
	float pi=0.0;
	NUM_PONTOS = strtoul(argv[2], NULL, 10);
	omp_set_num_threads(atoi(argv[1]));

	#pragma omp parallel private(i) reduction(+:pi)
	{
		#pragma omp for schedule(static)
		for(i=0;i<NUM_PONTOS;i++){
			pi += 4.0/(4.0*i+1.0);
			pi -= 4.0/(4.0*i+3.0);
		}
	}
			
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	printf("Pi = %lf\n", pi);
	return 0;
}

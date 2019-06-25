#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 4000000000\n\n");
		exit(1);
	}
	
	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	
	
	long long int i;
	float pi=0.0;
	long long int NUM_PONTOS = strtol(argv[1], NULL, 10);
	
	for(i=0;i<NUM_PONTOS;i++){
			pi += 4.0/(4.0*i+1.0);
			pi -= 4.0/(4.0*i+3.0);
	}
	
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	printf("%f\n", pi);
	return 0;
}

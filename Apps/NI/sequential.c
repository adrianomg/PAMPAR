//C program which estimates an integral by using an averaging technique

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


double f (double x){
	double pi = 3.141592653589793;
	double value;
	value = 50.0 / (pi * (2500.0 * x * x + 1.0));
	return value;
}

int main ( int argc, char *argv[] ){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 1000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	double a = 0.0;
	double b = 10.0;
	unsigned long int i; 
	unsigned long int n = strtoul(argv[1], NULL, 10);
	//unsigned long n = (argc<2)?1000:(unsigned long)atol(argv[1]);
	double total = 0.0;
	double x = 0.0;
	for(i = 0; i < n; i++ ){
		x = (( n - i - 1) * a + (i) *b) / (n - 1);
		total = total + f(x);
	}
	total = ( b - a ) * total / (double) n;

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	printf("%f\n", total);
	return 0;
}

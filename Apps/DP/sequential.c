#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv){

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 10000000000\n\n");
		exit(1);
	}	
	
	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	long long int N = strtoll(argv[1], NULL, 10);
	long long int i;
	long long int sum=0;

	for(i=0;i<N;i++){
			sum += i * (N-i);
	}
	
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	printf("%lld\n", sum);
	return 0;
}

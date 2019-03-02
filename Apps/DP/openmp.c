#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: my_program <threads> <input-size>\n\n \tE.g. -> ./my_program 2 10000000000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	
	long long int N = strtoul(argv[2], NULL, 10);
	long long int i;
	long long int sum=0;
	omp_set_num_threads(atoi(argv[1]));
	long long int chunk = N/(atoi(argv[1]));
	#pragma omp parallel private(i) reduction(+:sum)
	{
		#pragma omp for schedule(static)
		for(i=0;i<N;i++){
			sum += i * (N-i);
		}
	}
	//printf("%llu\n", sum);
	
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
}

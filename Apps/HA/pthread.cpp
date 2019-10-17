#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

using namespace std;
int numThreads, indice, inicio[64], fim[64];
long unsigned int d, n, *digits;
long long total_all = 0;
pthread_mutex_t mutex_sum;
pthread_barrier_t cond;

void ajusta(){
	if(n%numThreads == 0){
		inicio[0] = 1;
		fim[0] = n/numThreads;
	    for(long unsigned int i=1;i<numThreads;i++){
			  	inicio[i] = ((n/numThreads) * i) + 1;
			  	fim[i] = (n/numThreads)*(i+1);
       	}
	}else{
		long unsigned int sobra = n%numThreads, j=0;
		inicio[0] = 1;
		fim[0] = n/numThreads + 1;
		sobra = sobra - 1;	
		
		for(long unsigned int i=1;i<=numThreads;i++){
	      	if(i <= sobra){
		  		inicio[i] = (n/numThreads)*(i) + 2;
				fim[i] = (n/numThreads)*(i+1) + 2;
	    	}else{
				inicio[i] = (n/numThreads)*(i) + i+1;
				fim[i] = (n/numThreads)*(i+1) + i+1 - j;
    	  		j++;
			}
		}
	}
}

void *sum_Par(void *threadid){

	long unsigned int digits_local[d+11];
	for(long unsigned int digit = 0; digit < d+11; ++digit)
		digits_local[digit] = 0;
	long int idThread = (long int)threadid;
	for (long unsigned int i = inicio[idThread]; i <= fim[idThread]; ++i) {
		long unsigned int remainder = 1;
		/*Cálculo do resto da divisão*/
		for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
			long unsigned int div = remainder / i;
			long unsigned int mod = remainder % i;
			digits_local[digit] += div;
			remainder = mod * 10;
		}
	}

	pthread_barrier_wait(&cond);
	pthread_mutex_lock(&mutex_sum);
	for(long unsigned int digit = 0; digit < d + 11; ++digit)
		digits[digit] += digits_local[digit];
	pthread_mutex_unlock(&mutex_sum);
	return NULL;
}


void sum(char* output) {

	long unsigned int inicio[numThreads], fim[numThreads], qtdLinhas;
	long unsigned int digitsOp[d+11];
	long int i;	
	for(i=0; i < numThreads; i++){
		inicio[i] = 0;
		fim[i] = 0;
	}
	for (long unsigned int digit = 0; digit < d + 11; ++digit) {
		digits[digit] = 0;
	}

	ajusta();

    pthread_t threads[numThreads];
	for(i=0;i<numThreads;i++){
		pthread_create(&threads[i], NULL, sum_Par, (void *)i);
	}
	for(i=0;i<numThreads;i++){
			pthread_join(threads[i], NULL);
	}	

	for (long unsigned int i = d + 11 - 1; i > 0; --i) {
		digits[i - 1] += digits[i] / 10;
		digits[i] %= 10;
	}
	if (digits[d + 1] >= 5) {
		++digits[d];
	}
	for (long unsigned int i = d; i > 0; --i) {
		digits[i - 1] += digits[i] / 10;
		digits[i] %= 10;
	}
	stringstream stringstreamA;
	stringstreamA << digits[0] << ",";
	for (long unsigned int i = 1; i <= d; ++i) {
		stringstreamA << digits[i];
	}
	stringstreamA << '\0';
	string stringA = stringstreamA.str();
	stringA.copy(output, stringA.size());
}

int main(int argc, char **argv) {

	if (argc < 4) {
		printf ("ERROR! Usage: my_program <threads> <iterations> <input-size>\n\n \tE.g. -> ./my_program 2 100000 100000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	d = atoi(argv[2]);
	n = atoi(argv[3]);
	digits = (long unsigned int*)malloc((d+11)*sizeof(long unsigned int));
	numThreads = atol(argv[1]);
	pthread_barrier_init(&cond, NULL, numThreads);
	char output[d + 10]; // extra precision due to possible error

	sum(output);
		
	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif
		
	free(digits);
	return 0;

}

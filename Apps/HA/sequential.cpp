#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

void sum(char* output, const long unsigned int d, const long unsigned int n) {
	long unsigned int digits[d + 11];
	for (long unsigned int digit = 0; digit < d + 11; ++digit) {
		digits[digit] = 0;
	}
	for (long unsigned int i = 1; i <= n; ++i) {
		long unsigned int remainder = 1;
		for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
			long unsigned int div = remainder / i;
			long unsigned int mod = remainder % i;
			digits[digit] += div;
			remainder = mod * 10;
		}
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
	string stringA = stringstreamA.str();
	stringA.copy(output, stringA.size());
}

int main(int argc, char **argv) {

	if (argc < 2) {
		printf ("ERROR! Usage: my_program <input-size>\n\n \tE.g. -> ./my_program 100000\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	long unsigned int d, n;

	d = atoi(argv[1]);
	n = d;

	char output[d + 10]; // extra precision due to possible error

	sum(output, d, n);

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Execution time\t%f\n", delta);
	#endif

	return 0;
}

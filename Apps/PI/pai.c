#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

long long int NUM_PONTOS;
#define PAI 0

float calculaPi(int size){
        long long int j, chunk;
        float pi=0.0;
        chunk = NUM_PONTOS/size;
        for(j=0;j<chunk;j++){
                pi += 4.0/(4.0*j+1.0);
                pi -= 4.0/(4.0*j+3.0);
        }
        return pi;
}

int main(int argc, char **argv){

	if (argc < 3) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 4000000000 \"$PWD/child\"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif
	int pid, err[1];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	MPI_Comm interCommFilho[128];
	MPI_Request reqs[128];
	MPI_Status st[128];


	NUM_PONTOS = strtoul(argv[2], NULL, 10);
	char *bin;
	int tam1 = strlen(argv[argc-1]);
	bin = (char*)malloc((tam1)*sizeof(char));
	strcpy(bin, argv[argc-1]);

	int nFilhos = atoi(argv[1]);

	int i, j=0;
	float pi_local = 0.0;
	float pi_global = 0.0;

	argv[argc] = NULL;
	for(i=0; i<nFilhos; i++){
		MPI_Comm_spawn(bin, argv, 1, localInfo, PAI, MPI_COMM_SELF, &interCommFilho[i], err);	
		MPI_Irecv(&pi_global, 1, MPI_FLOAT, MPI_ANY_SOURCE, 99, interCommFilho[i], &reqs[i]);
	}
		
	pi_local = calculaPi(nFilhos+1);


	for(i=0;i<nFilhos;i++){
		MPI_Waitany(nFilhos, reqs, &j, MPI_STATUS_IGNORE);
		pi_local+=pi_global;
	}
		
	pi_local = pi_local/(nFilhos+1);

	MPI_Finalize();

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif
	
	printf("Pi = %f\n", pi_local);	

	return 0;

}

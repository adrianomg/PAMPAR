#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <sys/time.h>

long long int N;

void ajustaChunk(int size, long long int *vetIni, long long int *vetFim){
	long long int i, j, chunk, chunkPlus, sobra;
	if(N%size == 0){
		chunk = N/size;
		for(i=0;i<size;i++){
			vetIni[i] = i*chunk;
			vetFim[i] = (i*chunk)+chunk;
		}
	}else{
		chunk = N/size;
		chunkPlus = chunk+1;
		sobra = N%size;
		for(i=0;i<size;i++){
			if(i < sobra){
				vetIni[i] = i*chunkPlus;
				vetFim[i] = (i*chunkPlus)+chunkPlus;
			}else{
				vetIni[i] = vetFim[i-1];
				vetFim[i] = vetIni[i]+chunk;
			}
		}
	}
}

int main(int argc, char **argv){

	if (argc < 4) {
		printf ("ERROR! Usage: mpirun -np <n-father-proc> my_program <n-child-proc> <input-size> <child-exec>\n\n \tE.g. -> mpirun -np 1 ./my_program 3 10000000000 "$PWD/child"\n\n");
		exit(1);
	}

	#ifdef ELAPSEDTIME
		struct timeval start, end;
		gettimeofday(&start, NULL);
	#endif

	MPI_Init(&argc, &argv);
	MPI_Info localInfo;
	MPI_Info_create(&localInfo);
	int err[1], source=0;
	int num_Proc = atoi(argv[1]);
	MPI_Comm intercommFilho[num_Proc];
	MPI_Request req[num_Proc+2];
	int size = num_Proc+1;
		
	N = strtoul(argv[2], NULL, 10);
	char *bin;
    int tam1 = strlen(argv[argc-1]);
    bin = (char*)malloc((tam1)*sizeof(char));
    strcpy(bin, argv[argc-1]);

	long long int *vetIni, *vetFim;
	vetIni = malloc(sizeof(long long int)*size);
	vetFim = malloc(sizeof(long long int)*size);
	ajustaChunk(size, vetIni, vetFim);
	long long int i;
	int j;
	for(j=argc; j<10;j++){
		argv[j] = (char*) malloc(sizeof(char)*50);
	}
	for(j=0;j<num_Proc;j++){
		sprintf(argv[3], "%llu", vetIni[j+1]);
		sprintf(argv[4], "%llu", vetFim[j+1]);
		argv[5] = NULL;
		MPI_Comm_spawn(bin, argv, 1, localInfo, 0, MPI_COMM_SELF, &intercommFilho[j], err);
	}

	long long int sum_local=0, *sum_parcial = calloc(num_Proc, sizeof(long long int));
		
	for(j=0;j<num_Proc;j++){
	
		MPI_Irecv(&sum_parcial[j], 1, MPI_LONG, 0, 99, intercommFilho[j], &req[j]);
	}
	long long int ini = vetIni[0], fim = vetFim[0];

	for(i=ini;i<fim;i++){
		sum_local += i * (N-i);
	}
	for(j=0;j<num_Proc;j++){
		MPI_Waitany(num_Proc, req, &source, MPI_STATUS_IGNORE);
		sum_local += sum_parcial[source];
	}
	//printf("%llu\n", sum_local);
		
	MPI_Finalize();

	#ifdef ELAPSEDTIME
		gettimeofday(&end, NULL);
		double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		printf("Excution time\t%f\n", delta);
	#endif

	return 0;
	
}

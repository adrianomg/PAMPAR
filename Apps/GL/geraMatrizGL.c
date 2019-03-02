#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

int main(int argc, char **argv){
	long int nc, ni, i, j;
	FILE *out;
	nc = atoi(argv[1]);
	out = fopen(argv[2],"wt");
	fprintf(out,"%ld\n",nc);
	for(i=0;i<nc;i++){
		for(j=0;j<nc;j++){
			if(rand()/((float)RAND_MAX + 1) < 0.5)
                              fprintf(out,"%d ",0);
                        else
                              fprintf(out,"%d ",1);
		}
		fprintf(out,"\n");
	}
	free(out);
}

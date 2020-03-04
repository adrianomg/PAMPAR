#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

struct ring{
	int X, Y;
};
typedef struct ring TipoRing;

MPI_Comm intercommPai;
MPI_Request req[5];

void zeraAux(TipoRing *nova, int N){
        int i, j;
        for(i=0;i<N;i++){
                for(j=0;j<N;j++){
                        nova[(i*N)+j].X = 0;
                        nova[(i*N)+j].Y = 0;
                }
        }

}

void joga(TipoRing *jogo, int i, int j, int N, TipoRing *nova){
        int Xmenor=0, Xmaior=0, Ymenor=0, Ymaior=0, X, Y;
        float rx = 0.0798; //Taxa de morte Predadores por idade
        float ax = 0.0123; //Taxa de morte Predadores por superpopulação
        float bx = 0.0377; //Taxa de nascimento dos predadores
        float u = 0.12; //Taxa de migração dos predadores
        float ry = 0.0178; //Taxa de morte das presas
        float ay = 0.0998; //Taxa de presas mortas pelos predadores
        float by = 0.0100; //Taxa de nascimento das presas
        float v = 0.13; //Taxa de migração das presas

        float popX1, popY1, popX2, popY2;
        float fxy, gxy;

        X = jogo[(i*N)+j].X;
        Y = jogo[(i*N)+j].Y;

        fxy = X*(rx + (ax * X) + (bx * Y));
        gxy = Y*(ry + (ay * X) + (by * Y));

    	// verificando  posicao anterior
    	if ((i-1) < 0) {
    		Xmenor =  jogo[((N-1)*N)+j].X;
    	} else {
    		Xmenor = jogo[((i-1)*N)+j].X;
    	}

    	if ((i+1) > (N-1)) {
    		Xmaior = jogo[j].X;
    	} else {
    		Xmaior = jogo[((i+1)*N)+j].X;
    	}

    	popX1 = fxy +  u*(Xmenor - (2* X) + Xmaior);
    	popY1 = gxy +  v*(Ymenor - (2* Y) + Ymaior);

    	// verifica posicao final
    	if ((j-1) < 0) {
    		Ymenor =  jogo[(i*N)+N-1].Y;
    	} else {
    		Ymenor = jogo[(i*N)+j-1].Y;
    	}

    	if ((j+1) > (N-1)) {
    		Ymaior = jogo[i*N].Y;
    	} else {
    		Ymaior = jogo[(i*N)+j+1].Y;
    	}

    	popX2 = fxy +  u*(Xmenor - (2* X) + Xmaior);
    	popY2 = gxy +  v*(Ymenor - (2* Y) + Ymaior);

    	float Xfinal = (popX1 + popX2)/2;
    	float Yfinal = (popY1 + popY2)/2;

        nova[(i*N)+j].X = Xfinal;
        nova[(i*N)+j].Y = Yfinal;
}

void ajustaChunk(int *vetChunk, int *vetFim, int *vetIni, int size, int N){
        int i, chunk, chunkPlus, sobra;
        //não existem sobras
        if(N%size == 0){
                chunk = N/size;
                for(i=0;i<size;i++){
                        vetChunk[i] = chunk;
                        vetIni[i] = i*chunk;
                        vetFim[i] = vetIni[i] + chunk;
                }
        }else{
                chunk = N/size;
                chunkPlus = chunk+1;
                sobra = N%size;
                for(i=0;i<size;i++){
                        if(i < sobra){
                            vetIni[i] = i*chunkPlus;
                            vetFim[i] = (i*chunkPlus)+chunkPlus;
                            vetChunk[i] = chunkPlus;
                        }else{
                            vetIni[i] = vetFim[i-1];
                            vetFim[i] = vetIni[i]+chunk;
                            vetChunk[i] = chunk;
                        }
                }
        }
}

void ativaIrecv(int rank, int size, TipoRing *jogo, int *vetFim, int *vetIni, int N){
	if(size == 2){
		MPI_Irecv(&jogo[(vetFim[rank]-1)*N], N, MPI_INT, rank, 49, intercommPai, &req[0]);
	}else if(size > 2 && size % 2 == 0){
		if(rank != 0 && rank % 2 == 0 && rank != (size-2)){
			MPI_Irecv(&jogo[(vetFim[rank]-1)*N], N, MPI_INT, rank-1, (((rank-1)*100)+99), MPI_COMM_WORLD, &req[0]);
			MPI_Irecv(&jogo[(vetIni[rank+2]*N)], N, MPI_INT, rank+1, (((rank+1)*100)+49), MPI_COMM_WORLD, &req[1]);

		}else if(rank == 0){
			MPI_Irecv(&jogo[(vetFim[rank]-1)*N], N, MPI_INT, rank, 49, intercommPai, &req[0]);
			MPI_Irecv(&jogo[(vetIni[rank+2]*N)], N, MPI_INT, rank+1, (((rank+1)*100)+49), MPI_COMM_WORLD, &req[1]);

		}else if(rank == (size-2)){
			MPI_Irecv(&jogo[(vetFim[rank]-1)*N], N, MPI_INT, rank-1, (((rank-1)*100)+99), MPI_COMM_WORLD, &req[0]);

		}else if (rank % 2 != 0 && rank != (size-2)){
			MPI_Irecv(&jogo[(vetIni[rank+2]*N)], N, MPI_INT, rank+1, (((rank+1)*100)+49), MPI_COMM_WORLD, &req[0]); 
			MPI_Irecv(&jogo[(vetFim[rank]-1)*N], N, MPI_INT, rank-1, (((rank-1)*100)+99), MPI_COMM_WORLD, &req[1]);
		}
	}
}

void trocaDados(int rank, int size, TipoRing *jogo, int *vetFim, int *vetIni, int N){
				
	if(size == 2){
		MPI_Send(&jogo[(vetFim[rank+1])*N], N, MPI_INT, rank, 9, intercommPai);
		MPI_Wait(&req[0], MPI_STATUS_IGNORE);
	}else{
		if(rank == 0){
			MPI_Send(&jogo[(vetIni[rank+1]*N)], N, MPI_INT, rank, 9, intercommPai);
			MPI_Send(&jogo[(vetFim[rank+1]-1)*N], N, MPI_INT, rank+1, 99, MPI_COMM_WORLD);
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
			MPI_Wait(&req[1], MPI_STATUS_IGNORE);
		}else if(rank == (size-2)){
			MPI_Send(&jogo[(vetIni[rank+1]*N)], N, MPI_INT, rank-1, ((rank*100)+49), MPI_COMM_WORLD);
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
		}else{
			MPI_Send(&jogo[(vetIni[rank+1]*N)], N, MPI_INT, rank-1, ((rank*100)+49), MPI_COMM_WORLD);
			MPI_Send(&jogo[(vetFim[rank+1]-1)*N], N, MPI_INT, rank+1, ((rank*100)+99), MPI_COMM_WORLD);
			MPI_Wait(&req[0], MPI_STATUS_IGNORE);
			MPI_Wait(&req[1], MPI_STATUS_IGNORE);
		}
	}
}

void imprimeMatriz(TipoRing *jogo, int N){
        int i, j;
        printf("\n");
        for(i=0;i<N;i++){
                for(j=0;j<N;j++){
                        printf("%d|%d  ",jogo[(i*N)+j].X, jogo[(i*N)+j].Y);
                }
                printf("\n");
        }

}
int main(int argc, char **argv){
	argv+=1;
	int rank = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_get_parent(&intercommPai);

	int N = atoi(argv[2]);
	int tam = ((N*2)-1);
	int numFilhos = atoi(argv[1]);
	int size = numFilhos+1, *vetFim = NULL, *vetIni = NULL, *vetChunk = NULL, x = 0, linha = 0, coluna = 0, i = 0;
	TipoRing *jogo = malloc(sizeof(TipoRing)*N*N);
	TipoRing *auxiliar = malloc(sizeof(TipoRing)*N*N);
	vetFim = malloc(sizeof(int)*size);
	vetIni = malloc(sizeof(int)*size);
	vetChunk = malloc(sizeof(int)*size);

	zeraAux(auxiliar, N);
	zeraAux(jogo, N);

	ajustaChunk(vetChunk, vetFim, vetIni, size, N);
	
	MPI_Recv(&jogo[0], N*N, MPI_INT, 0, 49, intercommPai, MPI_STATUS_IGNORE);
        

	for(x=0;x<tam;x++){
		for(linha=vetIni[rank+1]; linha<vetFim[rank+1]; linha++){
			for(coluna=0;coluna<N;coluna++){
				if(coluna + linha == x){
					joga(jogo, linha, coluna, N, auxiliar);
				}
			}
		}

                //aciona MPI_Irecv//
                ativaIrecv(rank, size, jogo, vetFim, vetIni, N);

		for(linha=vetIni[rank+1]; linha<vetFim[rank+1]; linha++){
			for(coluna=0;coluna<N;coluna++){
				if(coluna + linha == x){
					jogo[(linha*N)+coluna].X = auxiliar[(linha*N)+coluna].X;
					jogo[(linha*N)+coluna].Y = auxiliar[(linha*N)+coluna].Y;
				}
			}
		}
		trocaDados(rank, size, jogo, vetFim, vetIni, N);
	}
	MPI_Send(&jogo[(vetIni[1]*N)], vetChunk[1]*N, MPI_INT, 0, 9, intercommPai);
        
	MPI_Finalize();
	return 0;
}

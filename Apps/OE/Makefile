############################# Makefile ##########################

all: sequential$(EXEC_EXT) openmp$(EXEC_EXT) pthread$(EXEC_EXT) mpi1$(EXEC_EXT) pai$(EXEC_EXT) filho$(EXEC_EXT)
	@echo "DONE!"

sequential$(EXEC_EXT): sequential.c
	$(CC) -o $@ $< $(MACRO) $(CFLAGS)

openmp$(EXEC_EXT): openmp.c
	$(CC) -o $@ $< -fopenmp $(MACRO) $(CFLAGS)

pthread$(EXEC_EXT): pthread.c
	$(CC) -o $@ $< -lpthread $(MACRO) $(CFLAGS)

mpi1$(EXEC_EXT): mpi1.c
	$(MPICC) -o $@ $< $(MACRO) $(CFLAGS)

pai$(EXEC_EXT): static_pai.c
	$(MPICC) -o $@ $< $(MACRO) $(CFLAGS)

filho$(EXEC_EXT): static_filho.c
	$(MPICC) -o $@ $< $(MACRO) $(CFLAGS)

#.PHONY: msg
#msg:
#	@echo "COMPILING $(PWD)..."

.PHONY: clean
clean:
	rm -rf *.btr
	rm -rf *.x*
	rm -rf sequential
	rm -rf openmp
	rm -rf pthread
	rm -rf mpi1
	rm -rf pai
	rm -rf filho

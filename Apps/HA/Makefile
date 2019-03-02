############################# Makefile ##########################

all: sequential$(EXEC_EXT) openmp$(EXEC_EXT) pthread$(EXEC_EXT) mpi1$(EXEC_EXT) pai$(EXEC_EXT) filho$(EXEC_EXT)
	@echo "DONE!"

sequential$(EXEC_EXT): sequential.cpp
	$(CPP) -o $@ $< $(MACRO) $(CFLAGS)

openmp$(EXEC_EXT): openmp.cpp
	$(CPP) -o $@ $< -fopenmp $(MACRO) $(CFLAGS)

pthread$(EXEC_EXT): pthread.cpp
	$(CPP) -o $@ $< -lpthread $(MACRO) $(CFLAGS)

mpi1$(EXEC_EXT): mpi1.cpp
	$(MPICPP) -o $@ $< $(MACRO) $(CFLAGS)

pai$(EXEC_EXT): pai.cpp
	$(MPICPP) -o $@ $< $(MACRO) $(CFLAGS)

filho$(EXEC_EXT): filho.cpp
	$(MPICPP) -o $@ $< $(MACRO) $(CFLAGS)

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

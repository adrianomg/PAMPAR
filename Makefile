#################################
### Compiler configuration:

CC=gcc
CPP=g++
MPICC=mpicc
MPICPP=mpic++
CFLAGS=-O2 -lm
MACRO=-DELAPSEDTIME
EXEC_EXT=

 
### End of compiler configuration
#################################

TARGETS=$(shell find Apps/* -maxdepth 0)

export CC CPP MPICC MPICPP CFLAGS MACRO CFLAGS EXEC_EXT

all: build
 
build:
	@set -e; for t in $(TARGETS); do make -C $$t; done
 
clean:
	#@make -C common clean
	@set -e; for t in $(TARGETS); do make -C $$t clean; done
  
.PHONY: build clean

# PAMPAR

**To cite this work:**

 - Garcia, A. M., Schepke, C., Girardi, A. PAMPAR: A new parallel benchmark for performance and energy consumption evaluation. Concurrency Computation Practice Experience. 2019;e5504. https://doi.org/10.1002/cpe.5504

Bibtex:

```bibtex
@article{GARCIA:PAMPAR:2020,
  author = {Marques Garcia, Adriano and Schepke, Claudio and Girardi, Alessandro},
  title = {PAMPAR: A new parallel benchmark for performance and energy consumption evaluation},
  journal = {Concurrency and Computation: Practice and Experience},
  volume = {32},
  number = {20},
  pages = {e5504},
  doi = {https://doi.org/10.1002/cpe.5504},
  url = {https://onlinelibrary.wiley.com/doi/abs/10.1002/cpe.5504},
  eprint = {https://onlinelibrary.wiley.com/doi/pdf/10.1002/cpe.5504},
  note = {e5504 cpe.5504},
  year = {2020}
}
```

PAMPAR is a parallel benchmark suite that provides a broad set of benchmarks. All are parallelized using three state-of-the-art parallel programming interfaces (parallel libraries), including two different MPI implementations (dynamic and static process spawning).

Currently, the suite consists of 13 parallel benchmarks (micro, kernels, and pseudo-applications), each parallelized using PThreads, OpenMP, MPI-1 (static process spawning), and MPI-2 (dynamic process spawning). It also offers a clean serial version to be used as a base benchmark and to ease extending it with implementations using new programming models.

**Table: Suite details.**
| Benchmark | Acronym | Set | Complexity | Domain |
| --- | :---: | :---: | :---: | --- |
| Pi Caluculation       | PI | Micro | O(n) | Math | 
| Dot Product           | DP | Micro | O(n) | Linear Algebra | 
| Numeric Integration   | NI | Micro | O(n) | Physics |
| Odd-Even Sort         | OE | Kernel | O(n²) | Sorting Algorithms |
| Harmonic Sums         | HA | Kernel | O(n×d) | Physics, Engineering |
| Disc. Fourier Transf. | DFT | Kernel | O(n²) | Digital Signal Processing |
| Matrix Multiplication | MM | Kernel | O(n³) | Linear Algebra |
| Gram-Schmidt Process  | GS | Kernel | O(n³) | Linear Algebra |
| Jacobi Method         | JA | Kernel | O(n³) | Linear Algebra |
| Dijkstra Shortest Path| DJ | Kernel | O(n³) | Graphs |
| Turing Ring           | TR | Pseudo-application | O(m×n²) | Chemistry, Biological Simulation|
| Histograms Similarity | SH | Pseudo-application | O(n³) | Image Processing, Pattern Recognition |
| Game of Life          | GL | Pseudo-application | O(n³) | Cellular Automata, Physics Simulation |
  
  
  (GL-MPI and SH still need some polishing. Any help is welcome!)

The goal for the future is to parallelize the set using more interfaces and add new benchmarks.

More details and evaluations on these applications can be found at: [Towards a benchmark for performance and power consumption evaluation of parallel programming interfaces](https://repositorio.unipampa.edu.br/jspui/handle/riu/4136) (master thesis)

**How to run:**
  
  - The most recent tests of the benchmarks were on a computer with Ubuntu 18.04.2 with Linux version 4.15.0-45 and using Intel ICC 18.0.1 compiler.

  - To compile all the pseudo-applications, go to the PAMPAR root directory and run:
  
	`~$ make`
    
  - You can run the 'run.sh' script and follow the steps to set up and run the benchmark.
  
  	`~$ bash run.sh`
    
  It will run the pseudo-applications according to the setup and give the execution time for each case.
  

**How to run manually:**

  - Compile the applications using the Makefile
  
  - To run a PThread or OpenMP application, run:
    
    	~$ ./<binary_file> <number_of_threads> <input_problem>
    
    Example: 
    	`~$ ./pthread 4 2048`
   
    
  - To run an MPI-1 application, run:
  
  	`~$ mpirun -np <number_of_processes> <binary_file> <input>`
    
    Example: 
    	`~$ mpirun -np 4 ./mpi1 2048`
    
    
  - To run an MPI-2 application, run:
 
 	`~$ mpirun -np 1 <binary_file> <number_of_child_processes> <input> <child_binary_file>`
    
    Example: 
    	`~$ mpirun -np 1 ./pai 3 2048 ./filho`
  
  
  **Workload classes**
  
PAMPAR has four workload classes, which are:
	
- Small	
- Medium	
- Large	
- Debug
		
These classes can be selected in the menu when running the run.sh script.
	
   **Examples of `<input>` parameters for each application using the "medium" class:**

	- DFT: 32368
	-  DJ: 2048 adjacency_matrix.txt
	-  DP: 15000000000
	-  GS: 1024
	-  HA: 100000 100000
	-  JA: 2048
	-  MM: 4096
	-  NI: 1000000000
	-  OE: 225000
	-  PI: 4000000000
	-  TR: 2048

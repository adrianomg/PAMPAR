# PAMPAR

**To cite this work:**

 - Garcia, A. M., Schepke, C., Girardi, A. PAMPAR: A new parallel benchmark for performance and energy consumption evaluation. Concurrency Computation Practice Experience. 2019;e5504. https://doi.org/10.1002/cpe.5504


In this project we aim to build a suite of parallel benchmarks to evaluate parallel programing interfaces (parallel models).

In our research we found out that there is no benchmark focused on comparing parallel programing interfaces.

This is a initial work and we still updating these applications. Currently, the suite consists of 13 parallel benchmarks (micro, kernels, and pseudo-applications), each parallelized using PThreads, OpenMP, MPI-1, and MPI-2 (dynamic processes creation). We also offer a clean serial version to be used as base result and to ease implementations using new programming models.

**Table: Suite details.**
| Benchmark | Acronym | Set | Complexity | Domain |
| --- | :---: | :---: | :---: | --- |
| Pi Caluculation       | PI | Micro | O(n) | Math | 
| Dot Produtct          | DP | Micro | O(n) | Linear Algebra | 
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

The goal is to paralelize the set using more interfaces and add new benchmarks according to the demand.

More details and studies on these applications can be found at: https://www.researchgate.net/project/A-Parallel-Benchmark-for-Performance-Evaluation-and-Energy-Consumption-in-Multicore-and-Manycore-Architectures


**How to run:**


  - To compile all the pseudo-applications, go to the PAMPAR root directory and run:
  
	`~$ make`
    
  - You can run the 'run.sh' script and follow the steps to setup and run the benchmark.
  
  	`~$ bash run.sh`
    
  It will run the pseudo-applications according to the setup and give the execution time for each case.
  

**How to run manually:**

  - Compile the applications using the Makefile
  
  - To run a PThread or OpenMP application, run:
    
    	~$ ./<executable> <number_of_threads> <input_problem>
    
    Example: 
    	`~$ ./pthread 4 2048`
   
    
  - To run a MPI-1 application, run:
  
  	`~$ mpirun -np <number_of_processes> <executable> <input_problem>`
    
    Example: 
    	`~$ mpirun -np 4 ./mpi1 2048`
    
    
  - To run a MPI-2 application, run:
 
 	`~$ mpirun -np 1 <executable> <number_of_child_processes> <input_problem> <child_executable>`
    
    Example: 
    	`~$ mpirun -np 1 ./pai 3 2048 ./filho`
  
  
  **Workload classes**
  
  PAMPAR has 3 classes of problems size and one aditional for debug purposes.
The classes names are:
	
- Small	
- Medium	
- Large	
- Debug
		
These classes can be selected in the menu when running the run.sh script.
	
   **Examples of <input_problem> parameters for each application using the "medium" class:**

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

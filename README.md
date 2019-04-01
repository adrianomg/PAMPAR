# PAMPAR
In this project we aim to build a parallel benchmark to evaluate parallel programing interfaces (or parallel APIs).

In our research we found out that there is no benchmark focused on comparing parallel programing interfaces. By that we mean a benchmark with at least a whole set of parallel applications implemented using various interfaces. 

This is a initial work. Currently the benchmark consists of 13 parallel pseudo-applications, each parallelized using PThreads, OpenMP, MPI-1, and MPI-2 (dynamic processes creation).

The pseudo-aplications are:
  - Pi Caluculation       (PI);
  - Dot Produtct          (DP);
  - Numeric Integration   (NI);
  - Odd-Even Sort         (OE);
  - Harmonic Sums         (HA)
  - Disc. Fourier Transf. (DFT);
  - Histograms Similarity (SH);
  - Game of Life          (GL);
  - Turing Ring           (TR);
  - Dijkstra Shortest Path(DJ);
  - Jacobi Method         (JA);
  - Matrix Multiplication (MM);
  - Gram-Schmidt Process  (GS);

The goal is to paralelize the set using more interfaces and add new pseudo-aplications according to the demand.

More details and studies on these applications can be found at: https://www.researchgate.net/project/A-Parallel-Benchmark-for-Performance-Evaluation-and-Energy-Consumption-in-Multicore-and-Manycore-Architectures

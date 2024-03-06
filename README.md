### What is this repository for? ###

C++ implementation of Distribution Sweeping, applied to the problem of orthogonal segment intersection reporting.

### How do I get set up? ###

To compile execute: g++ -std=c++11 experiments.cpp distribution_sweep.cpp k_way_mergesort.cpp utility.cpp.

All experimentation parameters are defined in main method of experiments.cpp, namely:

-Sample size.
-Sample distribution.
-Alfa (For a sample of size N, Alfa*N segments will be horizontal, and the rest vertical).
-Blocksize.
-Memory size.
-Number of iterations per setting.

.csv data files are automatically generated, containing iterations values and averages.

# Vector-Sum-with-MPI
Code which calculate the sum of a vector using parallel computation.

I wrote these codes after studying the one given at http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml. However, the original code generates the main vector on the root process and send pieces of it to the other processes. 

The code vector_sum.c works in a similar way and using more nodes in this does not improve the performance. 

The code vector_sum2.c defines how each vector piece will be generated in each process and sends only this information. This method improves the calculations while using more processes.

In both previous codes the root process does not computes any partial sum and therefore it makes one node unavailable to speed up the calculations.

In the code vector_sum3.c all processes (including the root) performs the calculations. It is used MPI_Reduce() to compute the total sum. Therefore, the code became simpler and faster.

Compile as: mpicc -o vector_sum vector_sum3.c -lm
Run as: time mpirun -n x vector_sum
with x = number of splits desired + root process. For example: if x = 3, the vector will be splited in two.

Acknowledgements: I would like to thanks Gilles Gouaillardet (https://stackoverflow.com/users/8062491/gilles-gouaillardet) for the helpful suggestion.

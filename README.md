# Vector-Sum-with-MPI
Code which calculate the sum of a vector using parallel computation.

I worte this code after studying the one given at http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml. However, the original code generates the main vector on the root process and send pieces of it to the other processes. Using more nodes in this way may not improve the performance. 

This code defines how each vector piece will be generated in each process and sends only this information. This method improves the calculations while using more processes.

Compile as: mpicc -o vector_sum vector_send.c -lm
Run as: time mpirun -n x vector_sum
with x = number of splits desired + root process. For example: if * = 3, the vector will be splited in two.

Acknowledgements: I would like to thanks Gilles Gouaillardet (https://stackoverflow.com/users/8062491/gilles-gouaillardet) for the helpful suggestion.

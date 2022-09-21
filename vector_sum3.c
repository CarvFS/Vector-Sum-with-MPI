/*
Based on the code presented at http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml

Code which calculate the sum of a vector using parallel computation.
In case of main vector does not split equally to all processes, the leftover is passed to process id 1.
Process id 0 is the root process. However, it will also perform part of calculations.

Each process will generate and calculate the partial sum of the vector values. It will be used MPI_Reduce() to calculate the total sum.
Since the processes are independent, the printing order will be different at each run.

compile as: mpicc -o vector_sum vector_sum.c -lm
run as: time mpirun -n x vector_sum

x = number of splits desired + root process. For example: if x = 3, the vector will be splited in two.

Acknowledgements: I would like to thanks Gilles Gouaillardet (https://stackoverflow.com/users/8062491/gilles-gouaillardet) for the helpful suggestion.
*/

#include<stdio.h>
#include<mpi.h>
#include<math.h>

#define vec_len 100000000
double vec2[vec_len];

int main(int argc, char* argv[]){
    // defining program variables
    int i;
    double sum, partial_sum;

    // defining parallel step variables
    int my_id, num_proc, ierr, an_id, root_process;
    int vec_size, rows_per_proc, leftover, num_2_gen, start_point;

    vec_size = 1e8; // defining the main vector size
    
    ierr = MPI_Init(&argc, &argv);

    root_process = 0;

    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    rows_per_proc = vec_size/num_proc; // getting the number of elements for each process.
    rows_per_proc = floor(rows_per_proc); // getting the maximum integer possible.
    leftover = vec_size - num_proc*rows_per_proc; // counting the leftover.

    if(my_id == 1){
        num_2_gen = rows_per_proc + leftover; // if there is leftover, it is calculate in process 1
        start_point = my_id*num_2_gen; // the corresponding position on the main vector
    }
    else{
        num_2_gen = rows_per_proc;
        start_point = my_id*num_2_gen; // the corresponding position on the main vector
    }

    partial_sum = 0;
    for(i = start_point; i < start_point + num_2_gen; i++){
        vec2[i] = pow(i,2)+1.0; // defining vector values
        partial_sum += vec2[i]; // calculating partial sum
    }

    printf("Partial sum of process id %d: %f.\n", my_id, partial_sum);

    MPI_Reduce(&partial_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, root_process, MPI_COMM_WORLD); // calculating total sum

    if(my_id == root_process){
        printf("Total sum is %f\n", sum);
    }

    ierr = MPI_Finalize();
    return 0;
    
}

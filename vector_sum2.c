/*
Based on the code presented at http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml

Code which calculate the sum of a vector using parallel computation.
In case of main vector does not split equally to all processes, the leftover is passed to process id 1.
Process id 0 is the root process. Therefore it does not count while passing information.

Each process will generate and calculate the partial sum of the vector values and send it back to the root process, which will calculate the total sum.
Since the processes are independent, the printing order will be different at each run.

compile as: mpicc -o vector_sum vector_send.c -lm
run as: time mpirun -n x vector_sum

x = number of splits desired + root process. For example: if * = 3, the vector will be splited in two.

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
    int my_id, num_proc, ierr, an_id, root_process; // id of process and total number of processes
    int vec_size, rows_per_proc, leftover, num_2_gen, start_point;

    ierr = MPI_Init(&argc, &argv);

    root_process = 0;

    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if(my_id == root_process){

        vec_size = 1e8; // defining main vector size

        rows_per_proc = vec_size / (num_proc - 1); // average values per process: using (num_proc - 1) because proc 0 does not count as a worker.
        rows_per_proc = floor(rows_per_proc); // getting the maximum integer possible.
        leftover = vec_size - (num_proc - 1)*rows_per_proc; // counting the leftover.

        // defining the number of data and position corresponding to main vector
        
        for(an_id = 1; an_id < num_proc; an_id++){
            if(an_id == 1){ // worker id 1 will have more values if there is any leftover.
                num_2_gen = rows_per_proc + leftover; // counting the amount of data to be generated.
                start_point = (an_id - 1)*num_2_gen; // defining corresponding initial position in the main vector.
            }
            else{
                num_2_gen = rows_per_proc;
                start_point = (an_id - 1)*num_2_gen + leftover; // defining corresponding initial position in the main vector for other processes if there is leftover.
            }

            ierr = MPI_Send(&num_2_gen, 1, MPI_INT, an_id, 1234, MPI_COMM_WORLD); // sending the information of how many data must be generated.
            ierr = MPI_Send(&start_point, 1, MPI_INT, an_id, 1234, MPI_COMM_WORLD); // sending the information of initial positions on main vector.
        }
        
        
        sum = 0;
        for(an_id = 1; an_id < num_proc; an_id++){
            ierr = MPI_Recv(&partial_sum, 1, MPI_DOUBLE, an_id, 4321, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving partial sum.
            sum = sum + partial_sum;
        }

        printf("Total sum = %f.\n", sum);
        
    }
    else{
        ierr = MPI_Recv(&num_2_gen, 1, MPI_INT, root_process, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving the information of how many data worker must generate.
        ierr = MPI_Recv(&start_point, 1, MPI_INT, root_process, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving the information of initial positions.

        // generate and sum vector pieces
        partial_sum = 0;
        for(i = start_point; i < start_point + num_2_gen; i++){
            vec2[i] = pow(i,2)+1.0;
            partial_sum = partial_sum + vec2[i];
        }

        printf("Partial sum of %d: %f\n",my_id, partial_sum);

        ierr = MPI_Send(&partial_sum, 1, MPI_DOUBLE, root_process, 4321, MPI_COMM_WORLD); // sending partial sum to root process.
               
    }

    ierr = MPI_Finalize();
    return 0;
    
}
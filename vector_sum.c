/*
Based on the code presented at http://condor.cc.ku.edu/~grobe/docs/intro-MPI-C.shtml

Code which splits a vector and send information to other processes.
In case of main vector does not split equally to all processes, the leftover is passed to process id 1.
Process id 0 is the root process. Therefore it does not count while passing information.

Each process will calculate the partial sum of vector values and send it back to root process, which will calculate the total sum.
Since the processes are independent, the printing order will be different at each run.

compile as: mpicc -o vector_sum vector_send.c -lm
run as: time mpirun -n x vector_sum

x = number of splits desired + root process. For example: if x = 3, the vector will be splited in two.
*/

#include<stdio.h>
#include<mpi.h>
#include<math.h>

#define vec_len 100000000
double vec1[vec_len];
double vec2[vec_len];

int main(int argc, char* argv[]){
    // defining program variables
    int i;
    double sum, partial_sum;

    // defining parallel step variables
    int my_id, num_proc, ierr, an_id, root_process; // id of process and total number of processes
    int num_2_send, num_2_recv, start_point, vec_size, rows_per_proc, leftover;

    ierr = MPI_Init(&argc, &argv);

    root_process = 0;

    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if(my_id == root_process){
        // Root process: Define vector size, how to split vector and send information to workers
        vec_size = 1e8; // size of main vector

        for(i = 0; i < vec_size; i++){
            //vec1[i] = pow(-1.0,i+2)/(2.0*(i+1)-1.0); // defining main vector...  Correct answer for total sum = 0.78539816339
            vec1[i] = pow(i,2)+1.0; // defining main vector... 
            //printf("Main vector position %d: %f\n", i, vec1[i]); // uncomment if youwhish to print the main vector
        }

        rows_per_proc = vec_size / (num_proc - 1); // average values per process: using (num_proc - 1) because proc 0 does not count as a worker.
        rows_per_proc = floor(rows_per_proc); // getting the maximum integer possible.
        leftover = vec_size - (num_proc - 1)*rows_per_proc; // counting the leftover.

        // spliting and sending the values
        
        for(an_id = 1; an_id < num_proc; an_id++){
            if(an_id == 1){ // worker id 1 will have more values if there is any leftover.
                num_2_send = rows_per_proc + leftover; // counting the amount of data to be sent.
                start_point = (an_id - 1)*num_2_send; // defining initial position in the main vector (data will be sent from here)
            }
            else{
                num_2_send = rows_per_proc;
                start_point = (an_id - 1)*num_2_send + leftover; // starting point for other processes if there is leftover.
            }
            
            ierr = MPI_Send(&num_2_send, 1, MPI_INT, an_id, 1234, MPI_COMM_WORLD); // sending the information of how many data is going to workers.
            ierr = MPI_Send(&vec1[start_point], num_2_send, MPI_DOUBLE, an_id, 1234, MPI_COMM_WORLD); // sending pieces of the main vector.
        }

        sum = 0;
        for(an_id = 1; an_id < num_proc; an_id++){
            ierr = MPI_Recv(&partial_sum, 1, MPI_DOUBLE, an_id, 4321, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving partial sum.
            sum = sum + partial_sum;
        }

        printf("Total sum = %f.\n", sum);

    }
    else{
        // Workers:define which operation will be carried out by each one
        ierr = MPI_Recv(&num_2_recv, 1, MPI_INT, root_process, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving the information of how many data worker must expect.
        ierr = MPI_Recv(&vec2, num_2_recv, MPI_DOUBLE, root_process, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieving main vector pieces.
        
        partial_sum = 0;
        for(i=0; i < num_2_recv; i++){
            //printf("Position %d from worker id %d: %d\n", i, my_id, vec2[i]); // uncomment if youwhish to print position, id and value of splitted vector
            partial_sum = partial_sum + vec2[i];
        }

        printf("Partial sum of %d: %f\n",my_id, partial_sum);

        ierr = MPI_Send(&partial_sum, 1, MPI_DOUBLE, root_process, 4321, MPI_COMM_WORLD); // sending partial sum to root process.
        
    }

    ierr = MPI_Finalize();
    
}

#include <mpi.h>
#include <stdio.h>


int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    int partner_rank = (world_rank + 1) % world_size;

    if (world_rank == world_size) {
        MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0,MPI_COMM_WORLD);
        printf("%d sent and incremented ping_pong_count ""%d to %d\n", world_rank, ping_pong_count,partner_rank);
    else if (world_rank == world_size) {
        MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0,MPI_COMM_WORLD);
        printf("%d sent and incremented ping_pong_count ""%d to %d\n", world_rank, ping_pong_count,partner_rank);
    } else {
        MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d received ping_pong_count %d from %d\n",world_rank, ping_pong_count, partner_rank);
    }



    // Finalize the MPI environment.
    MPI_Finalize();
}
#include <mpi.h>
#include <unistd.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
  int token;
  MPI_Init(&argc, &argv);
  int worldSize;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  int worldRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  if (worldRank != 0) {
    printf("Waitin...\n");
    MPI_Recv(&token, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("[AWOKEN] process %d received token %d from process %d\n", worldRank, token,
           worldRank - 1);
  } else {
    printf("Process 0 is seting the initial value!\n");
    token = 2;
  }

  if (worldRank != 0) {
    token = token * 2;
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  } else {
    printf("Process 0 is sending first message!\n");
    sleep(5);
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  }

  

  if (worldRank == 0) {
    printf("Process 0 is waiting for the message\n");
    MPI_Recv(&token, 1, MPI_INT, worldSize - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("[0] process %d received token %d from process %d\n", worldRank,
           token, worldSize - 1);
  }

  MPI_Finalize();
  return 0;
}

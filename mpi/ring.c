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
    MPI_Recv(&token, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("process %d received token %d from process %d\n", worldRank, token,
           worldRank - 1);
  } else {
    token = 2;
  }

  if (worldRank != 0) {
    token = token * 2;
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  } else {
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  }

  

  if (worldRank == 0) {
    MPI_Recv(&token, 1, MPI_INT, worldSize - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("process %d received token %d from process %d\n", worldRank,
           token, worldSize - 1);
  }

  MPI_Finalize();
  return 0;
}

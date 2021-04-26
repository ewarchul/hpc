#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>


int main(int argc, char *argv[]) {
  int64_t token;
  MPI_Init(&argc, &argv);
  int worldSize;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  int worldRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  if (worldRank != 0) {
    MPI_Recv(&token, 1, MPI_INT, worldRank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("process %d received token %ld from process %d\n", worldRank, token,
           worldRank - 1);
  } else {
    token = 1;
  }

  if (worldRank != 0) {
    token = token * worldRank;
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  } else {
    MPI_Send(&token, 1, MPI_INT, (worldRank + 1) % worldSize, 0,
             MPI_COMM_WORLD);
  }

  

  if (worldRank == 0) {
    MPI_Recv(&token, 1, MPI_INT, worldSize - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("process %d received token %ld from process %d\n", worldRank,
           token, worldSize - 1);
  }

  MPI_Finalize();
  return 0;
}

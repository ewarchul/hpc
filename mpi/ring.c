#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int procNumber;
  MPI_Comm_size(comm, &procNumber);
  int procId;
  MPI_Comm_rank(comm, &procId);

  long long int token = 0;
  if (procId != 0) {
    MPI_Recv(&token, 1, MPI_INT, procId - 1, 0, comm, MPI_STATUS_IGNORE);
    printf("[P_%d -> P_%d]:%lld\n", procId - 1, procId, token);
  } else {
    token = 12;
  }

  if (procId != 0) {
    token = token * procId;
    MPI_Send(&token, 1, MPI_INT, (procId + 1) % procNumber, 0, comm);
  } else {
    MPI_Send(&token, 1, MPI_INT, (procId + 1) % procNumber, 0, comm);
  }

  if (procId == 0) {
    MPI_Recv(&token, 1, MPI_INT, procNumber - 1, 0, comm, MPI_STATUS_IGNORE);
    printf("[P_%d -> P_%d]:%lld\n", procNumber - 1, procId, token);
  }

  MPI_Finalize();
  return 0;
}

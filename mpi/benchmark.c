#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: mpirun bench MESSAGE_SIZE EXP_NUM\n");
    exit(1);
  }
  int messageSize = atoi(argv[1]);
  int expNum = atoi(argv[2]);

  int *testArray = calloc(messageSize, sizeof(int));

  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Init(&argc, &argv);
  int procsNum;
  MPI_Comm_size(comm, &procsNum);
  int procId;
  MPI_Comm_rank(comm, &procId);

  for (int exp_ = 0; exp_ < expNum; ++exp_) {

    double startTime = MPI_Wtime();
    if (procId == 1) {
      MPI_Recv(testArray, messageSize, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);
      MPI_Send(testArray, messageSize, MPI_INT, 0, 0, comm);
    } else if (procId == 0) {
      MPI_Send(testArray, messageSize, MPI_INT, 1, 0, comm);
      MPI_Recv(testArray, messageSize, MPI_INT, 1, 0, comm, MPI_STATUS_IGNORE);
    } else {
      break;
    }
    double endTime = MPI_Wtime();
    if (procId == 0) {
      double executionTime = endTime - startTime;
      FILE *logFile = fopen("benchmark.csv", "a");
      fprintf(logFile, "%d,%d,%f\n", exp_, messageSize, executionTime);
      fclose(logFile);
    }
  }

  free(testArray);
  MPI_Finalize();
  return 0;
}

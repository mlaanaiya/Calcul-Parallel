#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{

  int value;
  int my_rank, size;
  int previous, next;
  MPI_Status status;

  // Initialize MPI environment
  MPI_Init(NULL, NULL);

  // Get number of processes and rank of current process
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // determine my neighbours according to my rank
  if (my_rank == 0)
  {
    previous = size - 1;
    next = my_rank + 1;
  }
  else if (my_rank == size - 1)
  {
    previous = my_rank - 1;
    next = 0;
  }
  else
  {
    previous = my_rank - 1;
    next = my_rank + 1;
  }

  // Initialize value to be transmitted
  value = 1;

  // The nodes, starting with node 0, transmit the value to each other,
  // each time multiplying it by 2.
  // At the end of the transmission, node 0 receives the value 2^(size-1)

  for (int i = 0; i < size; i++)
  {
    if (my_rank == i)
    {
      // If current process is sender, print its rank, the communication type, and value
      printf("Rank: %d, Send, Value: %d\n", my_rank, value);
      // Send value to next process in the ring
      MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }
    else if (my_rank == (i + 1) % size)
    {
      // If current process is receiver, receive value from previous process in the ring
      MPI_Recv(&value, 1, MPI_INT, previous, 0, MPI_COMM_WORLD, &status);
      // Multiply received value by 2
      value *= 2;
      // Print rank, communication type, and value
      printf("Rank: %d, Recv, Value: %d\n", my_rank, value);
    }
    // Wait for all processes to reach this point before proceeding to the next iteration
    MPI_Barrier(MPI_COMM_WORLD);
  }

  // Once all transmissions are done, process 0 prints the final value
  if (my_rank == 0)
  {
    printf("Rank: %d, Value: %d\n", my_rank, value);
  }

  // Finalize MPI environment
  MPI_Finalize();

  // Exit program with success status
  return EXIT_SUCCESS;
}

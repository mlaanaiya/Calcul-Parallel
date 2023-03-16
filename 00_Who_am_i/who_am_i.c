#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{

  int rank, size;
  int l;
  char name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv); // Initialisation de l'environnement MPI

  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Récupération du rang du processus courant
  MPI_Comm_size(MPI_COMM_WORLD, &size); // Récupération du nombre total de processus MPI lancés
  MPI_Get_processor_name(name, &l);     // Récupération du nom de la machine sur laquelle tourne le processus courant

  printf("Hello world from process %d of %d on processor named %s\n", rank, size, name);

  MPI_Finalize(); // Destruction de l'environnement MPI

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// perform the dot product between the two vectors x and y of size n
float dot(float x[], float y[], int n);

int main(int argc, char *argv[])
{

  int const local_data_size = 5;
  float local_x[local_data_size], local_y[local_data_size];
  float local_dot, global_dot1, global_dot2, reference;
  int borne;

  int my_rank, size;

  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  borne = size * local_data_size - 1;
  reference = (float)(borne * (borne + 1) * (2 * borne + 1) / 6);

  // Initialization of both local vectors with the same values
  // the global vectors would be [0, 1, ..., local_data_size -1]
  for (int i = 0; i < local_data_size; i++)
  {
    local_x[i] = (float)(local_data_size * my_rank + i);
    local_y[i] = (float)(local_data_size * my_rank + i);
    // printf("[MPI process %d] value[%d]: %f\n", my_rank, i, local_x[i]);
  }

  local_dot = dot(local_x, local_y, local_data_size);

  printf("[MPI process %d] my local dot product: %f\n", my_rank, local_dot);

  /* Two-step operation */

  global_dot1 = 0.0;

  // Step 1
  // Use a collective communication to compute the global dot product
  // in such a way that the node 0 gets this value
  float local_sum = local_dot;
  MPI_Reduce(&local_sum, &global_dot1, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Node 0 displays the global value and the reference (sum of first integer ^ 2)
  if (my_rank == 0)
  {
    printf("[MPI process %d] Two-step collective operation global dot product: %f == %f\n", my_rank, global_dot1, reference);
  }

  // Step 2
  // Use a collective communication to broadcast the global value on each node
  MPI_Bcast(&global_dot1, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  // A node i (i different from 0) displays the global value
  if (my_rank != 0)
  {
    printf("[MPI process %d] Two-step collective operation global dot product: %f\n", my_rank, global_dot1);
  }

  /* One-step operation */

  global_dot2 = 0;

  // Step 3
  // Now use one single collective communication to perfom both step 1 and 2
  MPI_Allreduce(&local_dot, &global_dot2, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);

  // Another node displays the global value
  if (my_rank == 1)
  {
    printf("[MPI process %d] One-step collective operation global dot product: %f\n", my_rank, global_dot2);
  }
  MPI_Finalize();

  return EXIT_SUCCESS;
}

float dot(float x[], float y[], int n)
{
  float res = 0.0;

  for (int i = 0; i < n; i++)
  {
    res += x[i] * y[i];
  }

  return res;
}

/* Explication :

    Dans la première étape, nous utilisons la fonction MPI_Reduce pour calculer la somme locale
    des produits scalaires sur chaque processus et les additionner sur le processus 0. Nous
    utilisons la fonction MPI_SUM pour que le réducteur additionne les valeurs.
    Le résultat est stocké dans la variable global_dot1 du processus 0.

    Ensuite, dans la deuxième étape, nous utilisons la fonction MPI_Bcast pour diffuser la valeur
    de global_dot1 sur tous les processus. Cette fonction permet de réduire la communication nécessaire
    pour synchroniser les processus. Les processus autres que le processus 0 affichent simplement le résultat.

    Dans la troisième étape, nous utilisons la fonction MPI_Allreduce pour effectuer à la fois les deux
    premières étapes en une seule étape. Cette fonction calcule la somme locale des produits scalaires
    sur chaque processus, additionne les résultats locaux sur tous les processus et distribue le résultat
    global sur tous les processus. Le résultat est stocké dans la variable global_dot2.

    Enfin, un autre processus affiche le résultat global.

*/
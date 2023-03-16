#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{

  int size;
  int my_rank;
  int data_size = -100;
  int *buffer_send, *buffer_recv;
  int tag;
  MPI_Status status;
  int l;
  char name[MPI_MAX_PROCESSOR_NAME];

  // Make sure that the command line has one argument (the size of the data)

  if (argc != 2)
  {
    printf("usage : limite <data size>\n");
    return EXIT_FAILURE;
  }

  MPI_Init(&argc, &argv);

  // Make sure exactly 2 MPI processes are used
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size != 2)
  {
    printf("%d MPI processes used, please use 2.\n", size);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Get_processor_name(name, &l);
  printf("process %d of %d on processor named %s\n", my_rank, size, name);

  // Prepare parameters

  data_size = atoi(argv[1]);
  printf("The size of the data is %d\n", data_size);

  buffer_send = (int *)malloc(data_size * sizeof(int));
  buffer_recv = (int *)malloc(data_size * sizeof(int));
  buffer_send[0] = (my_rank == 0) ? 12345 : 67890;

  tag = 0;

  if (my_rank == 0)
  {
    // node 0 sends its buffer buffer_send of size data_size to node 1
    MPI_Send(buffer_send, data_size, MPI_INT, 1, tag, MPI_COMM_WORLD);
    printf("MPI process %d sent value %d to MPI process %d.\n", my_rank, buffer_send[0], 1);

    // node 0 receives in its buffer buffer_recv data from node 1
    MPI_Recv(buffer_recv, data_size, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
    printf("MPI process %d received value %d from MPI process %d.\n", my_rank, buffer_recv[0], 1);
  }
  else
  {
    // node 1 sends its buffer buffer_send of size data_size to node 0
    MPI_Send(buffer_send, data_size, MPI_INT, 0, tag, MPI_COMM_WORLD);
    printf("MPI process %d sent value %d to MPI process %d.\n", my_rank, buffer_send[0], 0);

    // node 1 receives in its buffer buffer_recv data from node 0
    MPI_Recv(buffer_recv, data_size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
    printf("MPI process %d received value %d from MPI process %d.\n", my_rank, buffer_recv[0], 0);
  }

  free(buffer_send);
  free(buffer_recv);

  MPI_Finalize();

  return EXIT_SUCCESS;
}

/*
Questions :
  1. Rappelez pour quelle taille de message (petite, grande), MPI Send aura un comportement
     asynchrone (resp. synchrone) :

     En général, MPI Send a un comportement synchrone pour les petites tailles de messages,
     ce qui signifie que l'appel Send bloque l'exécution du programme jusqu'à ce que le message
     ait été effectivement envoyé et reçu par le processus destinataire.

     Pour les grandes tailles de messages, MPI Send peut avoir un comportement asynchrone,
     ce qui signifie que l'appel Send peut renvoyer immédiatement sans attendre que le message
     ait été effectivement envoyé. Cependant, cela dépend de la mise en œuvre MPI utilisée et
     des paramètres spécifiés lors de l'initialisation de l'environnement MPI. Par conséquent,
     il est important de vérifier la documentation de la mise en œuvre MPI utilisée pour
     connaître les détails de son comportement spécifique.

  2. Que va-t-il se passer quand votre programme, complété comme indiqué, sera appelé avec une
     taille de message qui fera que MPI Send sera synchrone ?

     Si le programme est appelé avec une taille de message qui fera que MPI Send sera synchrone,
     alors le programme sera bloqué jusqu'à ce que le message soit effectivement envoyé et reçu,
     car MPI_Send bloquera jusqu'à ce que le tampon de données soit copié dans le tampon de
     réception du processus destinataire. Cela signifie que l'exécution du programme sera suspendue
     jusqu'à ce que l'envoi soit terminé, ce qui peut prendre du temps pour les messages de grande taille.

  3. Estimez à 10 entiers près, la taille limite sur deux noeuds du même ordinateur ?

     La taille limite sur deux noeuds du même ordinateur est un peu près à 1010.

  4. Proposez une solution pour que l’échange entre les deux noeuds puissent se faire au delà de cette limite
     (plusieurs réponses possibles). Vous avez la possibilité de les tester en dehors de la séance

    Il existe plusieurs solutions pour que l'échange entre les deux nœuds puisse se faire au-delà de la limite de taille de message,
    voici quelques-unes :

    Utilisation de MPI_Sendrecv : MPI_Sendrecv est une fonction qui envoie et reçoit des messages en une seule opération.
    Cela permet de s'assurer que les deux opérations d'envoi et de réception sont effectuées en même temps, évitant ainsi
    les blocages. Cette solution peut être utilisée lorsque les deux nœuds ont besoin d'échanger des messages de même taille.

    Utilisation de MPI_Isend et MPI_Irecv : Ces fonctions sont des versions non bloquantes de MPI_Send et MPI_Recv,
    respectivement. Elles permettent d'envoyer et de recevoir des messages en arrière-plan, ce qui permet de poursuivre
    l'exécution du programme sans attendre la fin de l'envoi ou de la réception. Cette solution peut être utilisée pour les
    messages de grande taille, car elle permet de diviser le message en plusieurs parties et de les envoyer ou de les recevoir
    en arrière-plan.

    Utilisation de MPI_Sendrecv_replace : MPI_Sendrecv_replace est une fonction qui permet de remplacer le tampon d'envoi et
    de réception par un nouveau tampon. Cette fonction est utile pour les messages de grande taille, car elle permet de diviser
    le message en plusieurs parties et de les envoyer ou de les recevoir en utilisant le même tampon. Cela évite de devoir allouer
    des tampons supplémentaires pour stocker le message.

    Il est important de noter que la performance de ces solutions dépendra de plusieurs facteurs, tels que la taille des messages,
    la bande passante du réseau, la latence du réseau, etc. Il est donc important de tester ces solutions en dehors de la séance
    pour déterminer la meilleure solution pour votre application spécifique.

  5. Estimez à 10 entiers près, la taille limite sur deux noeuds de deux ordinateurs différents ?

     La taille limite sur deux noeuds de deux ordinateurs différents est un peu près à 1010.

*/
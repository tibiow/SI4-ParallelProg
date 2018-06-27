/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2006      Cisco Systems, Inc.  All rights reserved.
 *
 * Simple ring test program
 */

#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>

int N;              //taille d'une colonne de la matrice
int* matrice;       
int* vecteur;

void generatemodel(char* m,char* v){
  FILE *file = fopen ( m, "r" );
    if ( file != NULL )
    {
    int* numbers = {0};
    int current_number;
         
    fseek(file, 0, SEEK_END);
    long length_of_file = ftell(file);
        //printf("length_of_file = %d\n", length_of_file);
        fseek(file, 0, SEEK_SET);
        int number_of_numbers=0;
        numbers = malloc(sizeof(int)*length_of_file); 


        while(!feof(file)){
      if(fscanf(file,"%d", &current_number) == 1){
        numbers[number_of_numbers++]=current_number;
          }
      }

        //s->size=number_of_numbers;
      N=sqrt(number_of_numbers);
        //printf("s size = %d\n", N);
        matrice=malloc(number_of_numbers*sizeof(int));
        for (int i = 0; i < number_of_numbers; i++)
        {
          matrice[i]=numbers[i];
          //printf("number = %d\n",numbers[i] );
        }

      fclose ( file );
    }
    else
    {
       perror ( m ); /* why didn't the file open? */
    }
    file = fopen ( v, "r" );
    if ( file != NULL )
    {
    int* numbers = {0};
    int current_number;
         
    fseek(file, 0, SEEK_END);
    long length_of_file = ftell(file);
        //printf("length_of_file = %d\n", length_of_file);
        fseek(file, 0, SEEK_SET);
        int number_of_numbers=0;
        numbers = malloc(sizeof(int)*length_of_file); 


        while(!feof(file)){
      if(fscanf(file,"%d", &current_number) == 1){
        numbers[number_of_numbers++]=current_number;
          }
      }

        //printf("s size = %d\n", N);
        vecteur=malloc(number_of_numbers*sizeof(int));
        for (int i = 0; i < number_of_numbers; i++)
        {
          vecteur[i]=numbers[i];
          //printf("number = %d\n",numbers[i] );
        }

      fclose ( file );
    }
    else
    {
       perror ( v ); /* why didn't the file open? */
    }

}

int main(int argc, char *argv[])
{
    char* m = argv[1];
    char* v = argv[2];
    generatemodel(m,v);

    int rank, size, next, prev, message, tag = 201;

    /* Start up MPI */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("%d\n", rank);

    /* Calculate the rank of the next process in the ring.  Use the
       modulus operator so that the last process "wraps around" to
       rank zero. */

    next = (rank + 1) % size;
    prev = (rank + size - 1) % size;

    /* If we are the "master" process (i.e., MPI_COMM_WORLD rank 0),
       put the number of times to go around the ring in the
       message. */

    if (0 == rank) {
        message = 20;

        printf("Process 0 sending %d to %d, tag %d (%d processes in ring)\n",
               message, next, tag, size);
        MPI_Send(&message, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        printf("Process 0 sent to %d\n", next);
    }

    /* Pass the message around the ring.  The exit mechanism works as
       follows: the message (a positive integer) is passed around the
       ring.  Each time it passes rank 0, it is decremented.  When
       each processes receives a message containing a 0 value, it
       passes the message on to the next process and then quits.  By
       passing the 0 message first, every process gets the 0 message
       and can quit normally. */

    while (1) {
        MPI_Recv(&message, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        if (0 == rank) {
            --message;
            printf("Process 0 decremented value: %d\n", message);
        }

        MPI_Send(&message, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        if (0 == message) {
            printf("Process %d exiting\n", rank);
            break;
        }
    }

    /* The last process does one extra send to process 0, which needs
       to be received before the program can exit */

    if (0 == rank) {
        MPI_Recv(&message, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    /* All done */

    MPI_Finalize();
    return 0;
}
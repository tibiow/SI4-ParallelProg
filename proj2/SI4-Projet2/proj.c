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



int N;
int size_of_vecteur;
int* matrice;
int* vecteur;

/*
**  taken from previous project. It is used to parse input files.
*/
void generatemodel(char* m,char* v){
  FILE *file = fopen ( m, "r" );
    if ( file != NULL )
    {
    int* numbers = {0};
    int current_number;
         
    fseek(file, 0, SEEK_END);
    long length_of_file = ftell(file);
        fseek(file, 0, SEEK_SET);
        int number_of_numbers=0;
        numbers = malloc(sizeof(int)*length_of_file); 


        while(!feof(file)){
      if(fscanf(file,"%d", &current_number) == 1){
        numbers[number_of_numbers++]=current_number;
          }
      }

      N=number_of_numbers;
        matrice=malloc(number_of_numbers*sizeof(int));
        for (int i = 0; i < number_of_numbers; i++)
        {
          matrice[i]=numbers[i];
        }

      fclose ( file );
    }
    else
    {
       perror ( m ); 
    }
    file = fopen ( v, "r" );
    if ( file != NULL )
    {
    int* numbers = {0};
    int current_number;
         
    fseek(file, 0, SEEK_END);
    long length_of_file = ftell(file);
        fseek(file, 0, SEEK_SET);
        int number_of_numbers=0;
        numbers = malloc(sizeof(int)*length_of_file); 


        while(!feof(file)){
      if(fscanf(file,"%d", &current_number) == 1){
        numbers[number_of_numbers++]=current_number;
          }
      }
        size_of_vecteur = number_of_numbers;
        vecteur=malloc(number_of_numbers*sizeof(int));
        for (int i = 0; i < number_of_numbers; i++)
        {
          vecteur[i]=numbers[i];
        }

      fclose ( file );
    }
    else
    {
       perror ( v );
    }

}

int main(int argc, char *argv[])
{
    
    generatemodel(argv[1],argv[2]);


    int q, p, next, prev, tag = 201;

    /* Start up MPI */

    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &q);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int size_of_matrice_proc = N/p;
    int matrice_proc[size_of_matrice_proc];
    int vecteur_proc[size_of_vecteur];

    next = (q + 1) % p;
    prev = (q + p - 1) % p;

    int* tmp = malloc(sizeof(int));
    int number_of_result_per_node = size_of_vecteur/p;
    
    //initialize result_proc to 0
    int result_proc[number_of_result_per_node];
    for (int i = 0; i < number_of_result_per_node; i++)
    {
      result_proc[i]=0;
    }

    //First proc
    if (q==0){
      int result[p];
      //send vecteur to everybody
      for (int i = 0; i < size_of_vecteur; i++)
      {
        MPI_Send(&vecteur[i], 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        vecteur_proc[i]=vecteur[i];
      }

      //send the matrix
      for (int i = size_of_matrice_proc; i < N; i++)
      {
        MPI_Send(&matrice[i], 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      }
      // process results for the operation of the proc 
      for (int i = 0; i < size_of_matrice_proc; i++)
      {
        matrice_proc[i]=matrice[i];
        result_proc[i/size_of_vecteur] += matrice_proc[i] * vecteur_proc[i%size_of_vecteur];
      }
      for (int i = 0; i < number_of_result_per_node; i++)
      {
        result[i] = result_proc[i];
        printf("%d\n", result[i]);
      }
      //receive results one by one
      
      for (int i = number_of_result_per_node; i < size_of_vecteur; i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        result[i] = *tmp;
        printf("%d\n", result[i]);
      }

    }
    
    //last proc
    else if(q==p-1){
      //receive vecteur 
      for (int i = 0; i < size_of_vecteur; i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        vecteur_proc[i]=*tmp;
      }

      //we receive the end of the matrix and we process results
      for (int i = 0; i <  size_of_matrice_proc; i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        matrice_proc[i]=*tmp;
        result_proc[i/size_of_vecteur] += matrice_proc[i] * vecteur_proc[i%size_of_vecteur];
      }

      //send results one by one      
      for (int i = 0; i < (number_of_result_per_node*(p-2)); i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(tmp, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      }
      for (int i = 0; i < number_of_result_per_node; i++)
      {
        MPI_Send(&result_proc[i], 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      } 

    }
    
    //other process of the ring
    else{
      //send the vecteur for everyone and keep a copy of it for the compute
      for (int i = 0; i < size_of_vecteur; i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        vecteur_proc[i]=*tmp;
        MPI_Send(tmp, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      }


      //receive the matrix and process results
      for (int i = 0; i < size_of_matrice_proc; i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        matrice_proc[i]=*tmp;
        result_proc[i/size_of_vecteur] += matrice_proc[i] * vecteur_proc[i%size_of_vecteur];
      }
      //send the rest of the matrix to next
      for (int i = 0; i < (size_of_matrice_proc*(p-q-1)); i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(tmp, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      }
      //send results from previous to next
      for (int i = 0; i < (number_of_result_per_node*(q-1)); i++)
      {
        MPI_Recv(tmp, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(tmp, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
         
      }
      //send current results
      for (int i = 0; i < number_of_result_per_node; i++)
      {
        MPI_Send(&result_proc[i], 1, MPI_INT, next, tag, MPI_COMM_WORLD);
      }

    }



    
    MPI_Finalize();
    return 0;
}
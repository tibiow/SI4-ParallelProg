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



int size_of_matrice;
int size_of_vecteur;
int* matrice;
int* vecteur;
int* result;
int size_of_each_matrice;
int q, p, next, prev, tag = 201;

/*
**  taken from previous project. It is used to parse input files.
*/
void generatemodel(char* m,char* v){
  if(q==0){

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

        size_of_matrice=number_of_numbers;
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
}

void send_size_of_vecteur(){
  if (q==0){


    MPI_Send(&size_of_vecteur, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
  }
  else if(q==p-1){
    MPI_Recv(&size_of_vecteur, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    vecteur = malloc(size_of_vecteur*sizeof(int));
  }
  else{
          MPI_Recv(&size_of_vecteur, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      vecteur = malloc(size_of_vecteur*sizeof(int));
      MPI_Send(&size_of_vecteur, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
  }
}


void send_vecteur(){
  if (q==0){
          MPI_Send(vecteur, size_of_vecteur, MPI_INT, next, tag, MPI_COMM_WORLD);
  }
  else if(q==p-1){
      MPI_Recv(vecteur, size_of_vecteur, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

  }
  else{
      MPI_Recv(vecteur, size_of_vecteur, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(vecteur, size_of_vecteur, MPI_INT, next, tag, MPI_COMM_WORLD);
  }
}


void send_size_of_matrice(){
  if (q==0){
      MPI_Send(&size_of_matrice, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
    
  }
  else if(q==p-1){
      MPI_Recv(&size_of_matrice, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

  }
  else{
      MPI_Recv(&size_of_matrice, 1, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(&size_of_matrice, 1, MPI_INT, next, tag, MPI_COMM_WORLD);   
  }
}


void send_matrice(){
  if (q==0){
      size_of_each_matrice = size_of_matrice/p;

      int matrice_to_send[size_of_each_matrice];
      for (int i = 1; i < p; ++i)
      {
        for (int j = 0; j < size_of_each_matrice; ++j)
        {
          matrice_to_send[j]=matrice[(size_of_each_matrice)*i+j];
        }
        MPI_Send(matrice_to_send, size_of_each_matrice, MPI_INT, next, tag, MPI_COMM_WORLD);
      }
  }
  else if(q==p-1){
      size_of_each_matrice = size_of_matrice/p;
      matrice = malloc(size_of_matrice*sizeof(int));

      MPI_Recv(matrice, size_of_each_matrice, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);



  }
  else{
      size_of_each_matrice = size_of_matrice/p;
      matrice = malloc(size_of_matrice*sizeof(int));

      for (int i = 0; i < p-q-1; i++)
      {
        MPI_Recv(matrice, size_of_each_matrice, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(matrice, size_of_each_matrice, MPI_INT, next, tag, MPI_COMM_WORLD);
      }

      MPI_Recv(matrice, size_of_each_matrice, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
}


void compute(){
  if (q==0){
      result = calloc(size_of_vecteur,sizeof(int));
  }
  else{
      result = calloc(size_of_vecteur/p,sizeof(int));

  }
  #pragma omp parallel for
  for (int j = 0; j < size_of_matrice/size_of_vecteur; ++j)
  {
    for (int i = 0; i < size_of_vecteur; ++i)
    {
      result[j] += matrice[j*size_of_vecteur+i] * vecteur[i]; 
    }
  }
}


void send_to_first_process(){
  if (q==0){
      for (int i = 1; i < p; ++i)
      {
        MPI_Recv(vecteur, size_of_vecteur/p, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        
        for (int j = 0; j < size_of_vecteur/p; ++j)
        {
          result[(size_of_vecteur/p)*i+j]=vecteur[j];
        }
        
      }    
  }
  else{
      MPI_Send(result, size_of_vecteur/p, MPI_INT, next, tag, MPI_COMM_WORLD);
      for (int i = 0; i < q-1; i++)
      {
        MPI_Recv(result, size_of_vecteur/p, MPI_INT, prev, tag, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(result, size_of_vecteur/p, MPI_INT, next, tag, MPI_COMM_WORLD);
      } 
  }
}

int main(int argc, char *argv[])
{

    /* Start up MPI */

    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &q);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    

    generatemodel(argv[1],argv[2]);

    next = (q + 1) % p;
    prev = (q + p - 1) % p;

    send_size_of_vecteur();
    send_vecteur();
    send_size_of_matrice();
    send_matrice();
    compute();
    send_to_first_process();

    if (q==0)
    {
      for (int i = 0; i < size_of_vecteur; ++i)
      {
        printf("%d\n", result[i]);
      }
    }


    MPI_Finalize();
    return 0;
}
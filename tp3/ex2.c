#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
 
#include <assert.h>  


int global = 0;

void privateScope0(){
  #pragma omp for
  for (int i = 0; i < 10; i++)
  {
    global++;
  }
}


void privateScope1(){
  
  #pragma omp for private(global)
  for (int i = 0; i < 10; i++)
  {
    global++;
  }
}

void privateScope2(){
  #pragma omp for private(global)
  for (int i = 0; i < 10; i++)
  {
    privateScope2_sub();
  }
}

void privateScope2_sub(){
  global++;
}

int main(int argc, char const *argv[])
{
  printf("%d\n", global);
  privateScope2();
  printf("%d\n", global);

  return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

void parallelRegion(){
  #pragma omp parallel
  printf("%s\n","coucou" );
}


void parallelRegionForOutside(){
  for (int i = 0; i < 10; ++i)
  {
    printf("%s\n","coucouOutside" );
  }
}


void parallelRegionForInside(){
  #pragma omp parallel
  for (int i = 0; i < 10; ++i)
  {
    printf("%s\n","coucouInside" );
  }
}


void parallelRegionForInsideWithPragma(){
  #pragma omp for
  for (int i = 0; i < 10; ++i)
  {
    printf("%s\n","coucouInsideWithPragma" );
  }
}


int main(int argc, char const *argv[])
{
  parallelRegion();
  parallelRegionForOutside();
  parallelRegionForInside();
  parallelRegionForInsideWithPragma();
  return 0;
}
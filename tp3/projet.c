#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <limits.h>
#include <math.h>

#define TAILLE_MAX 1000

struct tablo {
  int * tab;
  int size;
};

int m ;//log(size_source)/log(2);

int max(int a, int b){
  if (a<b)
  {
    return b;
  }
  return a;
}

int add(int a, int b){
  return a + b;
}

void printArray(struct tablo * tmp) {
  printf("---- Array of size %i ---- \n", tmp->size);
  int size = tmp->size;
  int i;
  for (i = 0; i < size; ++i) {
    printf("%i ", tmp->tab[i]);
  }
  printf("\n");
}

struct tablo * allocateTablo(int size) {
  struct tablo * tmp = malloc(sizeof(struct tablo));
  tmp->size = size;
  tmp->tab = malloc(size*sizeof(int));
  tmp->tab[0]=0;
  return tmp;
}

void montee(struct tablo * source, struct tablo * destination,int (*func_to_use)(int,int)) {
  int size_destination = destination->size;
  int size_source = source->size;
  
  for (int i = size_destination-1; i > (size_destination/2)-1 ; i--)
  {
    destination->tab[i] = source->tab[i-size_source];
  }

  for (int l = m-1; l >= 0; l--)
  {
    int init = pow(2,l);
    int fin = pow(2,l+1)-1;

    #pragma omp parallel for
    for (int j = init; j <= fin; j++)
    {
      destination->tab[j] = func_to_use(destination->tab[2*j], destination->tab[2*j+1]);
    }
  }
}

void descente(struct tablo * a, struct tablo * b,int (*func_to_use)(int,int)) {
  b->tab[1]=0;
  for (int l = 1; l <= m; l++)
  {
    int init = pow(2,l);
    int fin = pow(2,l+1)-1;

    #pragma omp parallel for
    for (int j = init; j <= fin; j++)
    {
      if (j%2==0)
      {
        b->tab[j] = b->tab[j/2]; 
      }
      else{
        b->tab[j] = func_to_use( b->tab[j/2] ,a->tab[j-1]);
      }
    }
  }

}

void final(struct tablo * a, struct tablo *b,int (*func_to_use)(int,int)) {
  
  int init = pow(2,m);
  int fin = pow(2,m+1)-1;

  #pragma omp parallel for
  for (int j = init; j <= fin; j++)
  {
    b->tab[j] = func_to_use(b->tab[j],a->tab[j]);
  }
}


void parallel_maximum(struct tablo * source,struct tablo * psum, struct tablo *ssum,struct tablo * pmax, struct tablo *smax,struct tablo * m){
  struct tablo * Ms = allocateTablo(source->size);
  struct tablo * Mp = allocateTablo(source->size);
  
  #pragma omp parallel for
  for (int i = 0; i < source->size; i++)
  {
    Ms->tab[i] = pmax->tab[i] - ssum->tab[i] + source->tab[i];
    Mp->tab[i] = smax->tab[i] - psum->tab[i] + source->tab[i];
    m->tab[i] = Ms->tab[i] + Mp->tab[i] - source->tab[i];
  }
}

void find_sequence(struct  tablo * m,struct  tablo * source){
  int max = m->tab[0];
  int start_index = 0;
  int end_index = 0;
  for (int i = 0; i < m->size; i++)
  {
    if(max < m->tab[i]){
      start_index = i;
      end_index = i;
      max = m->tab[i];
    }
    else if(max == m->tab[i]){
      end_index = i;
    }
  }
  
  printf("%d ", max );
  for (int i = start_index; i <= end_index; i++)
  {
    printf("%d ", source->tab[i]);
  }
  printf("\n");

}


void generateArray(struct tablo * s, char *filename) {
  //construction d'un tableau pour tester
FILE *file = fopen ( filename, "r" );
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

      s->size=number_of_numbers;
      //printf("s size = %d\n", s->size);
      s->tab=malloc(s->size*sizeof(int));
      for (int i = 0; i < s->size; i++)
      {
        s->tab[i]=numbers[i];
      }

      m = log2(number_of_numbers);

      fclose ( file );
   }
   else
   {
      perror ( filename ); /* why didn't the file open? */
   }
}


void montee_descente_final(struct tablo * source, struct tablo * destination,int (*func_to_use)(int,int)){

  struct tablo * a = allocateTablo(source->size*2);
  struct tablo * b = allocateTablo(source->size*2);
  montee(source, a,func_to_use);
  //printArray(a);

  
  descente(a, b,func_to_use);
  //printArray(b);
   
  final(a,b,func_to_use);
  //printArray(b);
  for (int i = 0; i < destination->size; i++)
  {
    destination->tab[i] = b->tab[(b->size/2)+i];
  }
} 


int main(int argc, char **argv) {
  

  struct tablo source;
  generateArray(&source,argv[1]);


  //PSUM
  struct tablo * PSUM = allocateTablo(source.size);
  montee_descente_final(&source,PSUM,add);


  //SSUM
  struct tablo * SSUM = allocateTablo(source.size);
  //reverse tab
  struct tablo * reverse = allocateTablo(source.size);
  for (int i = 0; i < source.size ; i++)
  {
    reverse->tab[i]=source.tab[(source.size)-i-1];
  }
  montee_descente_final(reverse,SSUM,add);
  //reverse
  for (int i = 0; i < SSUM->size/2 ; i++)
  {
    int tmp = SSUM->tab[i];
    SSUM->tab[i] = SSUM->tab[(SSUM->size)-i-1];
    SSUM->tab[(SSUM->size)-i-1]= tmp;
  } 



  //SMAX
  struct tablo * SMAX = allocateTablo(source.size);
  struct tablo * reverse_PSUM = allocateTablo(source.size);

  for (int i = 0; i < PSUM->size ; i++)
  {
    reverse_PSUM->tab[i]=PSUM->tab[(PSUM->size)-i-1];
  }
  montee_descente_final(reverse_PSUM,SMAX,max);
  for (int i = 0; i < SMAX->size/2 ; i++)
  {
    int tmp = SMAX->tab[i];
    SMAX->tab[i] = SMAX->tab[(SMAX->size)-i-1];
    SMAX->tab[(SMAX->size)-i-1]= tmp;
  } 


  //PMAX
  struct tablo * PMAX = allocateTablo(source.size);
  montee_descente_final(SSUM,PMAX,max);

  //max
  struct tablo * result = allocateTablo(source.size);
  parallel_maximum(&source,PSUM,SSUM,PMAX,SMAX,result);

  find_sequence(result,&source);
}